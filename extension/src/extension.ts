import * as vscode from 'vscode';

import { Client } from './client';

export async function activate(context: vscode.ExtensionContext) {
  const output = vscode.window.createOutputChannel('Vanadium', 'log');
  const traceOutput = vscode.window.createOutputChannel('Vanadium Language Server');
  const client = new Client(output, traceOutput);

  context.subscriptions.push(
    vscode.commands.registerCommand('vanadiumd.restart', async () => {
      output.clear();
      await client.restart(context);
    })
  );

  const status = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right);
  status.text = `$(refresh) vanadiumd ${context.extension.packageJSON['version']}`;
  // status.backgroundColor = new vscode.ThemeColor('statusBarItem.warningBackground');
  status.command = 'vanadiumd.restart';
  context.subscriptions.push(status);

  vscode.window.onDidChangeActiveTextEditor((editor) => {
    if (editor?.document.languageId === 'ttcn3') {
      status.show();
    } else {
      status.hide();
    }
  });

  await client.start(context);
}

export async function deactivate(): Promise<void> {}
