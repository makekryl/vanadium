import * as vscode from 'vscode';

export const logger = vscode.window.createOutputChannel('Vanadium', { log: true });

// https://github.com/astral-sh/ruff-vscode/blob/305fedbc9644d59e71071d89b01838c2c6f2d8c0/src/common/logger.ts#L70
export class LazyOutputChannel implements vscode.OutputChannel {
  name: string;
  _channel: vscode.OutputChannel | undefined;

  constructor(name: string) {
    this.name = name;
  }

  get channel(): vscode.OutputChannel {
    if (!this._channel) {
      this._channel = vscode.window.createOutputChannel(this.name);
    }
    return this._channel;
  }

  append(value: string): void {
    this.channel.append(value);
  }

  appendLine(value: string): void {
    this.channel.appendLine(value);
  }

  replace(value: string): void {
    this.channel.replace(value);
  }

  clear(): void {
    this._channel?.clear();
  }

  show(preserveFocus?: boolean): void;
  show(column?: vscode.ViewColumn, preserveFocus?: boolean): void;
  show(column?: any, preserveFocus?: any): void {
    this.channel.show(column, preserveFocus);
  }

  hide(): void {
    this._channel?.hide();
  }

  dispose(): void {
    this._channel?.dispose();
  }
}
