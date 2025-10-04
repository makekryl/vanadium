import * as vscode from 'vscode';
import {
  LanguageClient,
  LanguageClientOptions,
  ServerOptions,
  TransportKind,
} from 'vscode-languageclient/node';
import { config } from './config';
import { logger } from './logger';

type LaunchOptions = {
  command: string;
  args: string[];
};

export class LsClient {
  private clientOptions: LanguageClientOptions;
  private client?: LanguageClient;

  constructor(outputChannel: vscode.OutputChannel, traceOutputChannel: vscode.OutputChannel) {
    this.clientOptions = {
      documentSelector: ['ttcn3'],
      outputChannel: outputChannel,
      traceOutputChannel: traceOutputChannel,
    };
  }

  private launchOptions!: LaunchOptions;
  private getLaunchOptions(ctx: vscode.ExtensionContext): LaunchOptions {
    return {
      command: `${ctx.extensionPath}/bin/vanadiumd`,
      args: config.get<string[]>('arguments'),
    };
  }

  async start(ctx: vscode.ExtensionContext): Promise<vscode.Disposable> {
    this.launchOptions = this.getLaunchOptions(ctx);
    const serverOptions: ServerOptions = {
      ...this.launchOptions,
      transport: TransportKind.stdio,
    };

    this.client = new LanguageClient(
      'ttcn3',
      'Vanadium TTCN-3 Language Client',
      serverOptions,
      this.clientOptions
    );

    logger.info('Starting language server...');
    await this.client.start();
    return new vscode.Disposable(() => {
      if (this.client) {
        this.client.stop();
      }
    });
  }

  async restart(ctx: vscode.ExtensionContext): Promise<vscode.Disposable> {
    if (!this.client) {
      return Promise.reject(new Error('Language client is not initialized'));
    }

    if (JSON.stringify(this.launchOptions) !== JSON.stringify(this.getLaunchOptions(ctx))) {
      logger.info(`Server launch options has been changed`);
      return this.start(ctx);
    }

    logger.info(`Restarting language server...`);
    await this.client.restart();
    return new vscode.Disposable(() => {});
  }
}
