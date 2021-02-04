/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "mate-terminal-menu-plugin.h"
#include <file-info.h>
#include <gio/gio.h>

#include <QAction>
#include <QUrl>

#include <QTranslator>
#include <QLocale>
#include <QApplication>

#include <QtConcurrent>
#include <QProcess>

#include <QDebug>

using namespace Peony;

static QString terminal_cmd = nullptr;
static QString m_uri = nullptr;

MateTerminalMenuPlugin::MateTerminalMenuPlugin(QObject *parent) : QObject (parent)
{
    QTranslator *t = new QTranslator(this);
    qDebug()<<"\n\n\n\n\n\n\ntranslate:"<<t->load(":/translations/peony-mate-terminal-extension_"+QLocale::system().name());
    QApplication::installTranslator(t);

    QtConcurrent::run([=]{
        GList *infos = g_app_info_get_all();
        GList *l = infos;
        while (l) {
            const char *cmd = g_app_info_get_executable(static_cast<GAppInfo*>(l->data));
            QString tmp = cmd;
            if (tmp.contains("terminal")) {
                terminal_cmd = tmp;
                if (tmp == "mate-terminal") {
                    break;
                }
            }
            l = l->next;
        }
        g_list_free_full(infos, g_object_unref);
    });
}

void MateTerminalMenuPlugin::openTerminal(){

    //qDebug()<<"triggered"<<uri;
    QUrl url = m_uri;
    auto directory = url.path().toUtf8().constData();
    gchar **argv = nullptr;
    g_shell_parse_argv (terminal_cmd.toUtf8().constData(), nullptr, &argv, nullptr);
    GError *err = nullptr;
    g_spawn_async (directory,
                   argv,
                   nullptr,
                   G_SPAWN_SEARCH_PATH,
                   nullptr,
                   nullptr,
                   nullptr,
                   &err);
    if (err) {
        qDebug()<<err->message;
        g_error_free(err);
        err = nullptr;
        //try again to open terminal
        //maybe this method is better,but still need more test to confirm
        tryOpenAgain();
    }
    g_strfreev (argv);
}

void MateTerminalMenuPlugin::tryOpenAgain()
{
    QUrl url = m_uri;
    auto absPath = url.path();
    qDebug() << "tryOpenAgain terminal url:" <<absPath;
    QProcess p;
    p.setProgram(terminal_cmd);
    p.setArguments(QStringList()<<"--working-directory"<<absPath);
//    p.startDetached();
    p.startDetached(p.program(), p.arguments());
    p.waitForFinished(-1);
}

QList<QAction *> MateTerminalMenuPlugin::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction *> actions;
    qDebug() << "terminal_cmd:" <<terminal_cmd <<uri;
    if (uri.startsWith("trash://") || uri.startsWith("recent://"))
    {
        return actions;
    }

    //set default terminal_cmd value for unfind terminal has no option issue
    //FIXME for unknow reason, did not find any terminal but it is actually exist
    if (terminal_cmd.isNull())
    {
        qWarning("open terminal option has not find any terminal, set as default value.");
        terminal_cmd = "mate-terminal";
    }

    if (types == MenuPluginInterface::DirectoryView || types == MenuPluginInterface::DesktopWindow) {
        if (selectionUris.isEmpty()) {
            m_uri = uri;
            //virtual path not show this option
            auto info = FileInfo::fromUri(uri);
            if (info->isVirtual())
                return actions;

            QAction *dirAction = new QAction(QIcon::fromTheme("utilities-terminal-symbolic"), tr("Open Directory in T&erminal"), nullptr);
            dirAction->connect(dirAction, &QAction::triggered, [=](){
                openTerminal();
            });
            actions<<dirAction;
        }
        if (selectionUris.count() == 1) {
            //select computer or trash, return
            if (selectionUris.first().startsWith("trash://")
                || selectionUris.first().startsWith("computer://"))
                return actions;

            //virtual path not show this option
            auto info = FileInfo::fromUri(selectionUris.first());
            if (info->isVirtual())
                return actions;

            if (info->isDir()) {
                m_uri = selectionUris.first();
                QAction *dirAction = new QAction(QIcon::fromTheme("utilities-terminal-symbolic"), tr("Open Directory in T&erminal"), nullptr);
                dirAction->connect(dirAction, &QAction::triggered, [=](){
                    openTerminal();
                });
                actions<<dirAction;
            }
        }
    }
    return actions;
}
