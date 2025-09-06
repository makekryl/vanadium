import * as vscode from 'vscode';

import { Client } from './client';
import { spawnSync } from 'child_process';

// https://github.com/prettier/prettier-vscode/blob/ba0c734448feeb7a27a8e48d8c282251141db7b0/src/PrettierEditService.ts#L372C1-L397C4
function minimalEdit(document: vscode.TextDocument, string1: string) {
  const string0 = document.getText();
  // length of common prefix
  let i = 0;
  while (i < string0.length && i < string1.length && string0[i] === string1[i]) {
    ++i;
  }
  // length of common suffix
  let j = 0;
  while (
    i + j < string0.length &&
    i + j < string1.length &&
    string0[string0.length - j - 1] === string1[string1.length - j - 1]
  ) {
    ++j;
  }
  const newText = string1.substring(i, string1.length - j);
  const pos0 = document.positionAt(i);
  const pos1 = document.positionAt(string0.length - j);

  return vscode.TextEdit.replace(new vscode.Range(pos0, pos1), newText);
}

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

  vscode.languages.registerDocumentFormattingEditProvider('ttcn3', {
    provideDocumentFormattingEdits(
      document: vscode.TextDocument,
      _options: vscode.FormattingOptions,
      token: vscode.CancellationToken
    ): vscode.TextEdit[] {
      const abortController = new AbortController();
      const process = spawnSync('ttcn3fmt', [document.fileName], {
        signal: abortController.signal,
      });
      token.onCancellationRequested(() => abortController.abort());

      const result = process.stdout.toString();
      return [minimalEdit(document, result)];
    },
  });

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
