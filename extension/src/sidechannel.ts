import * as vscode from 'vscode';
import { lsClient } from './extension';

export const showImportTree = async () => {
  const editor = vscode.window.activeTextEditor;
  if (!editor) {
    vscode.window.showErrorMessage('No active editor.');
    return;
  }

  const document = editor.document;

  if (document.languageId !== 'ttcn3') {
    vscode.window.showErrorMessage('This command only works for TTCN-3 files');
    return;
  }

  const result = await lsClient?.languageClient.sendRequest<string>('vanadiumd/buildImportTree', {
    textDocument: { uri: document.uri.toString() },
  });
  if (!result) {
    vscode.window.showErrorMessage('Failed to query import tree from the server');
    return;
  }

  const output = await vscode.workspace.openTextDocument({
    language: 'plaintext',
    content: result,
  });
  await vscode.window.showTextDocument(output);
};
