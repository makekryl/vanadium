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
  private exe: string | undefined;
  private onStartedCallbacks: Set<() => void> = new Set();

  constructor(outputChannel: vscode.OutputChannel, traceOutputChannel: vscode.OutputChannel) {
    this.outputChannel = outputChannel;
    this.traceOutputChannel = traceOutputChannel;
    this.clientOptions = {
      documentSelector: ['ttcn3'],
      outputChannel: this.outputChannel,
      traceOutputChannel: this.traceOutputChannel,
    };
  }

  async initialize(context: vscode.ExtensionContext): Promise<void> {
    const exe = '/workspaces/vanadium/build/Debug/bin/lsp/vanadiumd'; // TODO
    this.start(context, exe);
  }

  async start(_context: vscode.ExtensionContext, exe: string): Promise<void> {
    this.exe = exe;
    this.outputChannel.appendLine(`Resolved to ${this.exe}`);

    const serverOptions: ServerOptions = {
      run: {
        command: this.exe,
        args: [],
        transport: TransportKind.stdio,
      },
      debug: {
        command: this.exe,
        args: [],
        transport: TransportKind.stdio,
      },
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

  getCurrentExe(): string | undefined {
    return this.exe;
  }

  onStarted(callback: () => void): vscode.Disposable {
    if (this.exe) {
      callback();
      return new vscode.Disposable(() => {});
    }

    this.onStartedCallbacks.add(callback);
    return new vscode.Disposable(() => {
      this.onStartedCallbacks.delete(callback);
    });
  }

  async restart(_context: vscode.ExtensionContext): Promise<void> {
    if (!this.client) {
      return Promise.reject(new Error('Language client is not initialized'));
    }
    this.outputChannel.appendLine(`Restarting language server...`);
    return this.client.restart();
  }
}
