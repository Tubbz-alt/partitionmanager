/***************************************************************************
 *   Copyright (C) 2008 by Volker Lanz <vl@fidra.de>                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "gui/mainwindow.h"

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "ops/operation.h"
#include "util/globallog.h"

#include <unistd.h>
#include <signal.h>

static void registerMetaTypes()
{
	qRegisterMetaType<Operation*>("Operation*");
	qRegisterMetaType<log::Level>("log::Level");
}

static bool checkPermissions()
{
	if (geteuid() != 0)
		return KMessageBox::warningContinueCancel(NULL, i18nc("@info",
				"<para><warning>You do not have administrative privileges.</warning></para>"
				"<para>It is possible to run <application>%1</application> without these privileges. "
				"You will, however, <emphasis>not</emphasis> be allowed to apply operations.</para>"
				"<para>Do you want to continue running <application>%1</application>?</para>",
				KGlobal::mainComponent().aboutData()->programName()),
	 		i18nc("@title:window", "No Administrative Privileges"),
			KGuiItem(i18nc("@action:button", "Run Without Administrative Privileges")),
			KStandardGuiItem::cancel(),
			"runWithoutRootPrivileges") == KMessageBox::Continue;

	return true;
}

void unblockSigChild()
{
	sigset_t sset;
	sigemptyset(&sset);
	sigaddset(&sset, SIGCHLD);
	sigprocmask(SIG_UNBLOCK, &sset, NULL);
}

int main(int argc, char* argv[])
{
	KAboutData about(
			"partitionmanager",
			0,
			ki18nc("@title", "<application>KDE Partition Manager</application>"),
			VERSION,
			ki18nc("@title", "Manage your disks, partitions and file systems"),
			KAboutData::License_GPL,
			ki18nc("@info:credit", "(c) 2008 Volker Lanz")
		);
	about.addAuthor(ki18nc("@info:credit", "Volker Lanz"), KLocalizedString(), "vl@fidra.de");
	about.setHomepage("http://www.partitionmanager.org");

	KCmdLineArgs::init(argc, argv, &about);

	// workaround for https://bugs.launchpad.net/kdesudo/+bug/272427
	unblockSigChild();

	KApplication app;

	registerMetaTypes();
	if (!checkPermissions())
		return 0;

	MainWindow* mainWindow = new MainWindow();
	mainWindow->show();

	return app.exec();
}
