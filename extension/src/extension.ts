import * as vscode from 'vscode';

import { LsClient } from './client';

import * as ttcn3fmt from './ttcn3fmt';
import { LazyOutputChannel, logger } from './logger';
import { config } from './config';

let lsClient: LsClient | undefined;

export async function activate(context: vscode.ExtensionContext) {
  if (config.get<string>('formattingEngine') === 'ttcn3fmt') {
    const ttcn3fmtPath = ttcn3fmt.searchExecutable();
    if (ttcn3fmtPath) {
      logger.info(`Using ttcn3fmt found at '${ttcn3fmtPath}'`);
      context.subscriptions.push(
        vscode.languages.registerDocumentFormattingEditProvider('ttcn3', {
          provideDocumentFormattingEdits: ttcn3fmt.provideDocumentFormattingEdits,
        })
      );
    } else {
      logger.error('Cannot find ttcn3fmt in PATH');
    }
  }

  context.subscriptions.push(
    vscode.commands.registerCommand('vanadiumd.restart', async () => {
      if (!config.get<boolean>('enable') && !lsClient) {
        vscode.window
          .showInformationMessage(
            'Language features from vanadiumd are currently disabled. Would you like to enable them?',
            'Enable',
            'Close'
          )
          .then(async (choice) => {
            if (choice === 'Enable') {
              await config.update<boolean>('enable', true);
              await initializeLanguageServer(context);
            }
          });
        return;
      }
      await lsClient?.restart(context);
    })
  );

  if (config.get<boolean>('enable')) {
    await initializeLanguageServer(context);
  }
}

export async function deactivate(): Promise<void> {}

async function initializeLanguageServer(context: vscode.ExtensionContext) {
  const outputChannel = vscode.window.createOutputChannel('Vanadium Language Server', 'log');
  const traceOutputChannel = new LazyOutputChannel('Vanadium Language Server Trace');

  context.subscriptions.push(outputChannel);
  context.subscriptions.push(traceOutputChannel);
  context.subscriptions.push(logger);

  lsClient = new LsClient(outputChannel, traceOutputChannel);

  //

  const statusItem = (() => {
    const item = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right);
    item.text = `$(refresh) vanadiumd ${context.extension.packageJSON['version']}`;
    item.tooltip = 'Restart vanadiumd TTCN-3 language server';
    item.command = 'vanadiumd.restart';
    if (context.extensionMode === vscode.ExtensionMode.Development) {
      item.backgroundColor = new vscode.ThemeColor('statusBarItem.warningBackground');
    }
    return item;
  })();
  context.subscriptions.push(statusItem);

  const isDocumentTargeted = (document?: vscode.TextDocument) =>
    document && (document.languageId === 'ttcn3' || document.fileName === '.vanadiumrc.toml');
  if (isDocumentTargeted(vscode.window.activeTextEditor?.document)) {
    statusItem.show();
  }
  context.subscriptions.push(
    vscode.window.onDidChangeActiveTextEditor((editor) => {
      if (isDocumentTargeted(editor?.document)) {
        statusItem.show();
      } else {
        statusItem.hide();
      }
    })
  );

  context.subscriptions.push(await lsClient.start(context));

  context.subscriptions.push(
    vscode.workspace.onDidSaveTextDocument((document) => {
      if (!document.fileName.endsWith('.vanadiumrc.toml')) {
        return;
      }

      vscode.window
        .showInformationMessage(
          `Vanadium configuration files has been changed. Do you like to restart Vanadium Language Server?`,
          'Restart',
          'Not now'
        )
        .then((selection) => {
          if (selection === 'Restart') {
            lsClient?.restart(context);
          }
        });
    })
  );
}
