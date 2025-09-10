import * as vscode from 'vscode';

const getConfig = () => vscode.workspace.getConfiguration('vanadiumd');

export const config = {
  get: <T>(key: string) => getConfig().get<T>(key)!,
  update: <T>(key: string, value: T, target?: vscode.ConfigurationTarget) =>
    getConfig().update(key, value, target),
};
