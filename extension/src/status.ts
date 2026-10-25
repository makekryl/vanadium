import * as vscode from 'vscode';
import { LsClient } from './client';
import { State } from 'vscode-languageclient';

export const createStatusItem = (context: vscode.ExtensionContext, lsClient: LsClient) => {
  const statusItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right);
  statusItem.text = `$(refresh) vanadiumd ${context.extension.packageJSON['version']}`;
  statusItem.command = 'vanadiumd.restart';
  //
  const tooltip = new vscode.MarkdownString();
  const setTooltip = (markdownText: string) => {
    tooltip.value = markdownText;
    statusItem.tooltip = tooltip;
  };

  const cleanStatus = () => setTooltip('Restart vanadiumd TTCN-3 language server');
  lsClient.languageClient.onDidChangeState((event) => {
    if (event.newState !== State.Running) {
      cleanStatus();
    }
  });

  const buildReportedStatus = ((pre, post) => (report: string) => {
    return `${pre}${report}${post}`;
  })(
    '**TTCN-3 language server**\n\n' + //
      `Client version: \`${context.extension.packageJSON['version']}\` \ \n`,
    /* report */
    '\n' + //
      '\n---\n' +
      '_Click to restart the language server_'
  );

  lsClient.languageClient.onNotification('$/vanadiumd/status', (report) => {
    setTooltip(buildReportedStatus(report));
  });

  if (context.extensionMode === vscode.ExtensionMode.Development) {
    statusItem.backgroundColor = new vscode.ThemeColor('statusBarItem.warningBackground');
  }

  return statusItem;
};
