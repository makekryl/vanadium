import * as vscode from 'vscode';

import { LsClient } from './client';

import * as ttcn3fmt from './ttcn3fmt';
import { LazyOutputChannel, logger } from './logger';
import { config } from './config';

let lsClient: LsClient | undefined;

export async function activate(context: vscode.ExtensionContext) {
  if (config.get<string>('formattingEngine') === 'ttcn3fmt') {
    if (ttcn3fmt.checkAvailability()) {
      logger.info('ttcn3fmt has been detected in the environment');
      context.subscriptions.push(
        vscode.languages.registerDocumentFormattingEditProvider('ttcn3', {
          provideDocumentFormattingEdits: ttcn3fmt.provideDocumentFormattingEdits,
        })
      );
    } else {
      logger.info('Cannot find ttcn3fmt in PATH');
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

  const status = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right);
  status.text = `$(refresh) vanadiumd ${context.extension.packageJSON['version']}`;
  status.tooltip = 'Restart vanadiumd TTCN-3 language server';
  if (context.extensionMode === vscode.ExtensionMode.Development) {
    status.backgroundColor = new vscode.ThemeColor('statusBarItem.warningBackground');
  }
  status.command = 'vanadiumd.restart';
  context.subscriptions.push(status);

  const isDocumentTargeted = (document?: vscode.TextDocument) =>
    document && (document.languageId === 'ttcn3' || document.fileName === '.vanadiumrc.toml');
  if (isDocumentTargeted(vscode.window.activeTextEditor?.document)) {
    status.show();
  }
  context.subscriptions.push(
    vscode.window.onDidChangeActiveTextEditor((editor) => {
      if (isDocumentTargeted(editor?.document)) {
        status.show();
      } else {
        status.hide();
      }
    })
  );

  await lsClient.start(context);
}
