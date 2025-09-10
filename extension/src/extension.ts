import * as vscode from 'vscode';

import { LsClient } from './client';

import * as ttcn3fmt from './ttcn3fmt';
import { LazyOutputChannel, logger } from './logger';

export async function activate(context: vscode.ExtensionContext) {
  const outputChannel = vscode.window.createOutputChannel('Vanadium Language Server', 'log');
  const traceOutputChannel = new LazyOutputChannel('Vanadium Language Server Trace');

  context.subscriptions.push(outputChannel);
  context.subscriptions.push(traceOutputChannel);
  context.subscriptions.push(logger);

  const lsClient = new LsClient(outputChannel, traceOutputChannel);

  context.subscriptions.push(
    vscode.commands.registerCommand('vanadiumd.restart', async () => {
      outputChannel.clear();
      await lsClient.restart(context);
    })
  );

  const status = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right);
  status.text = `$(refresh) vanadiumd ${context.extension.packageJSON['version']}`;
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

  if (ttcn3fmt.checkAvailability()) {
    logger.info('ttcn3fmt is detected in the environment');
    context.subscriptions.push(
      vscode.languages.registerDocumentFormattingEditProvider('ttcn3', {
        provideDocumentFormattingEdits: ttcn3fmt.provideDocumentFormattingEdits,
      })
    );
  }

  await lsClient.start(context);
}

export async function deactivate(): Promise<void> {}
