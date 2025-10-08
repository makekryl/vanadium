import * as vscode from 'vscode';
import * as os from 'os';
import { execSync, spawnSync } from 'child_process';
import { logger } from './logger';

const TTCN3FMT_BIN = 'ttcn3fmt';

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

export const searchExecutable = (): string | null => {
  if (os.platform() === 'win32') {
    return null;
  }
  try {
    return execSync(`which ${TTCN3FMT_BIN}`).toString().trim();
  } catch {
    return null;
  }
};

export const provideDocumentFormattingEdits = (
  document: vscode.TextDocument,
  _options: vscode.FormattingOptions,
  token: vscode.CancellationToken
): vscode.TextEdit[] => {
  const abortController = new AbortController();
  token.onCancellationRequested(() => abortController.abort());

  const process = spawnSync(`cat | ${TTCN3FMT_BIN} /dev/stdin`, [], {
    signal: abortController.signal,
    shell: true,
    input: document.getText(),
    timeout: 5000,
  });

  if (process.status !== 0) {
    logger.warn(
      `Failed to format code using ttcn3fmt (${process.status}):\n${process.stderr
        .toString()
        .trim()
        .split('\n')
        .map((line) => ` [ttcn3fmt] ${line}`)
        .join('\n')}`
    );
    return [];
  }

  return [minimalEdit(document, process.stdout.toString())];
};
