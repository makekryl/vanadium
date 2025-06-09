import * as vscode from 'vscode';

import { Client } from './client';

export async function activate(context: vscode.ExtensionContext) {
  const output = vscode.window.createOutputChannel('Vanadium', 'log');
  const traceOutput = vscode.window.createOutputChannel('Vanadium TTCN-3 Language Server');
  const client = new Client(output, traceOutput);

  // TODO: register commands (restart server, ...)

  context.subscriptions.push(
    vscode.commands.registerCommand('vanadiumd.restart', async () => {
      await client.restart(context);
    })
  );

  await client.initialize(context);
}

export async function deactivate(): Promise<void> {}
