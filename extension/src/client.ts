import * as vscode from 'vscode';
import {
  LanguageClient,
  LanguageClientOptions,
  ServerOptions,
  TransportKind,
} from 'vscode-languageclient/node';

export class Client {
  private outputChannel: vscode.OutputChannel;
  private traceOutputChannel: vscode.OutputChannel;
  private clientOptions: LanguageClientOptions;
  private client?: LanguageClient;

  constructor(outputChannel: vscode.OutputChannel, traceOutputChannel: vscode.OutputChannel) {
    this.outputChannel = outputChannel;
    this.traceOutputChannel = traceOutputChannel;
    this.clientOptions = {
      documentSelector: ['ttcn3'],
      outputChannel: this.outputChannel,
      traceOutputChannel: this.traceOutputChannel,
    };
  }

  async start(ctx: vscode.ExtensionContext): Promise<void> {
    const serverOptions: ServerOptions = {
      command: `${ctx.extensionPath}/bin/vanadiumd`,
      args: [],
      transport: TransportKind.stdio,
    };

    this.client = new LanguageClient(
      'ttcn3',
      'Vanadium TTCN-3 Language Client',
      serverOptions,
      this.clientOptions,
      true
    );

    this.outputChannel.appendLine(`Starting language server...`);
    await this.client.start();
  }

  async restart(_ctx: vscode.ExtensionContext): Promise<void> {
    if (!this.client) {
      return Promise.reject(new Error('Language client is not initialized'));
    }
    this.outputChannel.appendLine(`Restarting language server...`);
    return this.client.restart();
  }
}
