//
// Created by snownf on 24-8-22.
//

#ifndef CONTROL_PANEL_UTILS_H
#define CONTROL_PANEL_UTILS_H


class Utils {
public:
    static int exec(const QString &program, const QStringList &arguments, QString &result);

    static void unableToExecMsgBox(const QString &program, const QStringList &arguments, const QString &result);

    static QString cmdToString(const QString &program, const QStringList &arguments);

    static bool unableToExecContinueMsgBox(const QString &program, const QStringList &arguments, const QString &result);
};


#endif //CONTROL_PANEL_UTILS_H
