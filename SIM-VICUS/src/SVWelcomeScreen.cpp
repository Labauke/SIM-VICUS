/*	SIM-VICUS - Building and District Energy Simulation Tool.

	Copyright (c) 2020-today, Institut für Bauklimatik, TU Dresden, Germany

	Primary authors:
	  Andreas Nicolai  <andreas.nicolai -[at]- tu-dresden.de>
	  Dirk Weiss  <dirk.weiss -[at]- tu-dresden.de>
	  Stephan Hirth  <stephan.hirth -[at]- tu-dresden.de>
	  Hauke Hirsch  <hauke.hirsch -[at]- tu-dresden.de>

	  ... all the others from the SIM-VICUS team ... :-)

	This program is part of SIM-VICUS (https://github.com/ghorwin/SIM-VICUS)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

#include "SVWelcomeScreen.h"
#include "ui_SVWelcomeScreen.h"

#include <QDesktopServices>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTextStream>
#include <QNetworkAccessManager>
#include <QPixmap>

#include <VICUS_Project.h>
#include <VICUS_Constants.h>

#include <IBK_Version.h>
#include <IBK_BuildFlags.h>

#include <QtExt_LanguageHandler.h>
#include <QtExt_Directories.h>

#include "SVConstants.h"
#include "SVSettings.h"
#include "SVStyle.h"

extern const char * const HTML_TEMPLATE;
extern const char * const HTML_NEWS_TEMPLATE;
extern const char * const RECENT_PROJECT_TABLE_TEMPLATE;

SVWelcomeScreen::SVWelcomeScreen(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::SVWelcomeScreen),
	m_networkManager(new QNetworkAccessManager),
	m_autoUpdater(new QtExt::AutoUpdater(this)),
	m_pageType(PT_RecentFiles)
{
	m_ui->setupUi(this);
	connect(m_networkManager, SIGNAL(finished(QNetworkReply*)),
			this, SLOT(downloadFinished(QNetworkReply*)));
	connect(m_ui->textBrowserMain, SIGNAL(anchorClicked(QUrl)),
			this, SLOT(onAnchorClicked(QUrl)));
	connect(m_autoUpdater, SIGNAL(updateInfoRetrieved(int, QString)),
			this, SLOT(onUpdateInfoRetrieved(int, QString)) );
	if (UPDATE_FILE_URL != QString("..."))
		m_autoUpdater->checkForUpdateInfo(UPDATE_FILE_URL,
									  VICUS::LONG_VERSION, false, QString(), QString());

	// format text browser
	m_ui->textBrowserMain->setStyleSheet("QTextBrowser { border: none; }");
	m_ui->textBrowserNews->setStyleSheet("QTextBrowser { border: none; }");

	m_ui->toolButtonNewProject->setIcon(QIcon::fromTheme("file_new"));
	m_ui->toolButtonOpenProject->setIcon(QIcon::fromTheme("file_open"));

	// labels need a fixed width to prevent shifting the layout during hovering (bold text is wider)
	// we set the width dynamically here, as it depends on translation
	m_ui->labelRecent->setActive(true);
	QSize s1 = m_ui->labelRecent->sizeHint();
	m_ui->labelRecent->setFixedWidth(s1.width());
	m_ui->labelExample->setActive(true);
	QSize s2 = m_ui->labelExample->sizeHint();
	m_ui->labelExample->setFixedWidth(s2.width());

	connect(m_ui->labelRecent, SIGNAL(clicked()), this, SLOT(on_labelRecentClicked()));
	connect(m_ui->labelExample, SIGNAL(clicked()), this, SLOT(on_labelExampleClicked()));

	on_labelRecentClicked();

	// hide news for now
	m_ui->textBrowserNews->setVisible(false);

}


SVWelcomeScreen::~SVWelcomeScreen() {
	delete m_ui;
	delete m_networkManager;
}


void SVWelcomeScreen::updateWelcomePage() {
	// news text
	QString htmlPageNews = HTML_NEWS_TEMPLATE;
	QString welcomePageSideBar = QString("<p>&nbsp;<p><hr>"); // top-rule starts the side-bar column
	// add update info section
	if (!m_updateInfoNews.isEmpty())
		welcomePageSideBar.append(m_updateInfoNews);
	// download news content (only once per application start)
	if (m_welcomePageNews.isEmpty()) {
		QNetworkRequest request(QUrl(QString::fromLatin1(NEWS_FILE_URL)));
		/*QNetworkReply *reply = */m_networkManager->get(request);
	}
	// insert news
	if (m_welcomePageNews.isEmpty()) {
		welcomePageSideBar.append(QString("<i>%1</i>").arg(tr("Retrieving news content...")));
	}
	else {
		welcomePageSideBar.append(m_welcomePageNews);
	}
	htmlPageNews.replace("${NEWS_CONTENT}", welcomePageSideBar);
	SVStyle::setHtmlColors(htmlPageNews);
	m_ui->textBrowserNews->setHtml(htmlPageNews);

	// now main page
	QString htmlPage = HTML_TEMPLATE;
	SVStyle::setHtmlColors(htmlPage);

	int thumbNailSize = (int)SVSettings::instance().m_thumbNailSize;

	if (m_pageType == PT_RecentFiles) {
		// compose recent project file list table

		QString infoString;

		for (int i=0; i<SVSettings::instance().m_recentProjects.count(); ++i) {
			QFileInfo finfo(SVSettings::instance().m_recentProjects[i]);
			QString projectInfoBlock = RECENT_PROJECT_TABLE_TEMPLATE;
			QString description;
			// check if file still exists
			if (finfo.exists()) {
				VICUS::Project pro;
				try {
					pro.parseHeader(IBK::Path(finfo.filePath().toStdString()));
				}
				catch (IBK::Exception &) {
					// error reading project file, missing permissions?
					pro = VICUS::Project(); // reset with empty instance
				}
				QString created;
				if (pro.m_projectInfo.m_created.empty())
					created = "---";
				else
					created = QString::fromStdString(pro.m_projectInfo.m_created);
				QString lastModified;
				if (pro.m_projectInfo.m_lastEdited.empty())
					lastModified = "---";
				else
					lastModified = QString::fromStdString(pro.m_projectInfo.m_lastEdited);
				description = tr("<i>Created: %1</i><br><i>Last modified: %2</i><br>%3")
									  .arg(created, lastModified, QString::fromStdString(pro.m_projectInfo.m_comment));
			}
			else {
				description = tr("<i><font color=\"${STYLE_H3_COLOR}\">Project not accessible</font></i> <a href=\"premove:%1\">Remove %2 from list</a>").arg( i ).arg( finfo.fileName() );
				SVStyle::setHtmlColors(description);
			}

			// thumb name is full file path with replces
			QString thumbName = SVSettings::instance().m_recentProjects[i];
			thumbName.replace("/", "_");
			thumbName.replace("\\", "_");
			thumbName.replace(":", "_");
			thumbName.replace(".vicus", "");
			QString thumbPath = QtExt::Directories::userDataDir()  + "/thumbs/" + thumbName + ".png";

			QFileInfo thumbFileInfo(thumbPath);
			// check if file exists
			if (thumbFileInfo.exists())
				thumbPath = "<a href=\"pfile:${PROJECT_FULL_PATH}\"><img width=\"" + QString::number(thumbNailSize) + "\" src=\"" + thumbFileInfo.absoluteFilePath() + "\"></a>&nbsp;";
			else
				thumbPath = "&nbsp;";
			thumbPath = thumbPath.replace("${PROJECT_FULL_PATH}", finfo.filePath());

			// thumbnails generated from the software have all the same thumbnail size
			projectInfoBlock = projectInfoBlock.replace("${THUMBNAILSIZE}", QString("%1").arg(thumbNailSize+20));
			projectInfoBlock = projectInfoBlock.replace("${PROJECT_FILENAME}", finfo.fileName());
			projectInfoBlock = projectInfoBlock.replace("${PROJECT_FULL_PATH}", finfo.filePath());
			projectInfoBlock = projectInfoBlock.replace("${PROJECT_DESCRIPTION}", description);
			projectInfoBlock = projectInfoBlock.replace("${IMG_FILENAME}", thumbPath);
			infoString += projectInfoBlock;
		}

		htmlPage.replace("${RECENT_PROJECT_FILELIST}", infoString);
		m_ui->textBrowserMain->setHtml(htmlPage);
	}

//	else if(m_pageType == PT_TemplateFiles) {
//		QString templates = "<h1>" + tr("Project templates") + "</h1><p>\n";

//		// process all project packages in template directory
//		QDir templatesDir(QtExt::Directories::userDataDir()+"/DB_templates");

//		QStringList templateFiles;
//		SVSettings::recursiveSearch(templatesDir, templateFiles,
//									QStringList() << SVSettings::instance().m_projectPackageSuffix.mid(1));

//		// for each file, parse its header, extract comments, open thumbnail image and compose table with information
//		foreach (QString fname, templateFiles) {
//			QString projectInfoBlock = RECENT_PROJECT_TABLE_TEMPLATE;
//			// for templates we use a special link handler
//			projectInfoBlock.replace("pfile:$", "ptemplate:$");
//			VICUS::Project pro;
//			QFileInfo finfo(fname);
//			try {
//				pro.parseHeader(IBK::Path(finfo.filePath().toStdString()));
//			}
//			catch (IBK::Exception &) {
//				// error reading project file, missing permissions?
//				pro = VICUS::Project(); // reset with empty instance
//			}
//			QString created;
//			if (pro.m_projectInfo.m_created.empty())
//				created = "---";
//			else
//				created = QString::fromStdString(pro.m_projectInfo.m_created);
//			QString description = QString::fromStdString(pro.m_projectInfo.m_comment);

//			projectInfoBlock = projectInfoBlock.replace("${PROJECT_FILENAME}", finfo.baseName());
//			projectInfoBlock = projectInfoBlock.replace("${PROJECT_FULL_PATH}", finfo.filePath());
//			projectInfoBlock = projectInfoBlock.replace("${PROJECT_DESCRIPTION}", description);

//			// open thumbnail image for this project
//			QString thumbPath = fname+".png";

//			QFileInfo thumbFileInfo(thumbPath);
//			QPixmap p;
//			// check if file exists
//			if (thumbFileInfo.exists() && p.load(thumbPath)) {
//				/// \todo Andreas: fix warning about bad resource loading (related to image files)
//				thumbPath = "<a href=\"pfile:${PROJECT_FULL_PATH}\"><img width=\"" + QString::number(size) + "\" src=\"" + thumbFileInfo.absoluteFilePath() + "\"></a>&nbsp;";
//				thumbPath = thumbPath.replace("${PROJECT_FULL_PATH}", finfo.filePath());
//				// Example projects may have larger thumbnails - hence we set the thumbnailsize based on the image size
//				projectInfoBlock = projectInfoBlock.replace("${THUMBNAILSIZE}", QString("%1").arg(p.width()+20));
//			}
//			else {
//				thumbPath = "&nbsp;";
//				projectInfoBlock = projectInfoBlock.replace("${THUMBNAILSIZE}", QString("%1").arg(thumbNailSize+20));
//			}

//			projectInfoBlock = projectInfoBlock.replace("${IMG_FILENAME}", thumbPath);
//			templates += projectInfoBlock;
//		}

//		// TODO
////		htmlPage.replace("${RECENT_PROJECT_FILELIST}", templates);
//	}
	else if (m_pageType==PT_DemoFiles) {

		QString infoString;

		// process all files in examples directory
		QDir examplesDir(QtExt::Directories::databasesDir() + "/examples");

		QStringList exampleFiles;
		SVSettings::recursiveSearch(examplesDir, exampleFiles,
									QStringList() << SVSettings::instance().m_projectFileSuffix.mid(1));

		// for each file, parse its header, extract comments, open thumbnail image and compose table with information
		foreach (QString fname, exampleFiles) {
			QString projectInfoBlock = RECENT_PROJECT_TABLE_TEMPLATE;
			// for examples we use a special link handler
			projectInfoBlock.replace("pfile:$", "pexample:$");
			VICUS::Project pro;
			QFileInfo finfo(fname);
			try {
				pro.parseHeader(IBK::Path(finfo.filePath().toStdString()));
			}
			catch (IBK::Exception &) {
				// error reading project file, missing permissions?
				pro = VICUS::Project(); // reset with empty instance
			}
			QString created;
			if (pro.m_projectInfo.m_created.empty())
				created = "---";
			else
				created = QString::fromStdString(pro.m_projectInfo.m_created);
			QString description = QString::fromStdString(pro.m_projectInfo.m_comment);

			QString parentDirName = QFileInfo(finfo.dir().absolutePath()).fileName();
			projectInfoBlock = projectInfoBlock.replace("${PROJECT_FILENAME}", parentDirName + " - " + finfo.fileName());
			projectInfoBlock = projectInfoBlock.replace("${PROJECT_FULL_PATH}", finfo.filePath());
			projectInfoBlock = projectInfoBlock.replace("${PROJECT_DESCRIPTION}", description);

			// open thumbnail image for this project
			QString thumbPath = fname+".png";

			QFileInfo thumbFileInfo(thumbPath);
			QPixmap p;
			// check if file exists
			if (thumbFileInfo.exists() && p.load(thumbPath)) {
				/// \todo Andreas: fix warning about bad resource loading (related to image files)
				thumbPath = "<a href=\"pexample:${PROJECT_FULL_PATH}\"><img width=\"" + QString::number(thumbNailSize) + "\" src=\"" + thumbFileInfo.absoluteFilePath() + "\"></a>&nbsp;";
				thumbPath = thumbPath.replace("${PROJECT_FULL_PATH}", finfo.filePath());

				// Example projects may have larger thumbnails - hence we set the thumbnailsize based on the image size
				projectInfoBlock = projectInfoBlock.replace("${THUMBNAILSIZE}", QString("%1").arg(thumbNailSize+20));
			}
			else {
				thumbPath = "&nbsp;";
				projectInfoBlock = projectInfoBlock.replace("${THUMBNAILSIZE}", QString("%1").arg(thumbNailSize+20));
			}

			projectInfoBlock = projectInfoBlock.replace("${IMG_FILENAME}", thumbPath);
			infoString += projectInfoBlock;
		}

		htmlPage.replace("${RECENT_PROJECT_FILELIST}", infoString);
		m_ui->textBrowserMain->setHtml(htmlPage);
	}
}


void SVWelcomeScreen::setLabelColors(QString color) {
	m_ui->labelRecent->setStyleSheet("QLabel { font-weight: normal}", "QLabel { font-weight: bold}", QString("QLabel { font-weight: bold; color: %1}").arg(color));
	m_ui->labelExample->setStyleSheet("QLabel { font-weight: normal", "QLabel { font-weight: bold}", QString("QLabel { font-weight: bold; color: %1}").arg(color));
}


void SVWelcomeScreen::onAnchorClicked( const QUrl & link ) {
	// if anchor starts with "pfile:" we emit the "open project" signal
	if (link.toString().startsWith("pfile:")) {
		QString fname = link.toString();
		fname = fname.right(fname.length()-QString("pfile:").size());
		emit openProject(fname);
		return;
	}
	// if anchor starts with "pexample:" we emit the "open example" signal
	else if (link.toString().startsWith("pexample:")) {
		QString fname = link.toString();
		fname = fname.right(fname.length()-QString("pexample:").size());
		emit openExample(fname);
		return;
	}
	// if anchor starts with "ptemplate:" we emit the "open template" signal
	else if (link.toString().startsWith("ptemplate:")) {
		QString fname = link.toString();
		fname = fname.right(fname.length()-QString("ptemplate:").size());
		emit openTemplate(fname);
		return;
	}
	else if (link.toString().startsWith("premove:")) {

		// extract project index to delete and remove it from list
		QString index = link.toString();
		index = index.right(index.length()-QString("premove:").size());
		bool ok;
		int ind = index.toInt(&ok);
		if (ok && ind < SVSettings::instance().m_recentProjects.size()){
			SVSettings::instance().m_recentProjects.removeAt(ind);
		}

		// refresh view afterwards
		updateWelcomePage();

		// refresh menue structure
		emit updateRecentList();

		return;
	}
	else if (link.toString().startsWith("update:")) {
		emit softwareUpdateRequested();
		return;
	}

	// must be an external link, call desktop services to open webbrowser
	QDesktopServices::openUrl(link);
}


void SVWelcomeScreen::on_toolButtonNewProject_clicked() {
	emit newProjectClicked();
}

void SVWelcomeScreen::on_toolButtonOpenProject_clicked() {
	emit openProjectClicked();
}

void SVWelcomeScreen::downloadFinished(QNetworkReply *reply) {
	if (reply->error()) {
		m_welcomePageNews = QString("<i>%1</i>")
							.arg(tr("unable to download news content."));
	}
	else {
		QByteArray newsRaw = reply->readAll();
		// extract text for current language
		QString news = QString::fromUtf8(newsRaw);
		bool languageFound = false;
		if (news.indexOf("[lang:") != -1) {
#if QT_VERSION >= 0x050e00
			QStringList langTexts = news.split(QString("[lang:"), Qt::SkipEmptyParts);
#else
			QStringList langTexts = news.split(QString("[lang:"), QString::SkipEmptyParts);
#endif
			for (int i=0; i<langTexts.count(); ++i) {
				QString langId = langTexts[i].left(2);
				if (langId == QtExt::LanguageHandler::instance().langId()) {
					news = langTexts[i].right(langTexts[i].count()-3);
					languageFound = true;
					break;
				}
			}
			if (!languageFound && langTexts.count()>0) {
				// language not found, default to the first language (should be english)
				news = langTexts[0].right(langTexts[0].count()-3);
			}
		}

		// store news text
		m_welcomePageNews = news;
	}

	reply->deleteLater();
	// this will trigger a welcome page update (but no further download attempt)
	updateWelcomePage();
}


void SVWelcomeScreen::onUpdateInfoRetrieved(int res, QString newVersion) {
	// generate update info text, if update information was successfully retrieved
	// first set current version text
	if (res == 0) {
		// if we have a newer version, add html text with update link
		if (IBK::Version::lesserVersionNumber(VICUS::LONG_VERSION, newVersion.toStdString())) {
			m_updateInfoNews = tr("<h1><font style=\"color:${STYLE_H1_COLOR}\">An update is available!</font></h1><p>");
			m_updateInfoNews += tr("Current version: <b>%1</b><br>").arg(VICUS::LONG_VERSION);
			m_updateInfoNews += tr("New Version: <b><font style=\"color:${STYLE_LINKTEXT_COLOR}\">%1</font></b></p><p><a href=\"update:\">Download update</a>").arg(newVersion);
			m_updateInfoNews += "</p><hr>";
			SVStyle::setHtmlColors(m_updateInfoNews);
		}
	}
	updateWelcomePage();
}


void SVWelcomeScreen::on_labelRecentClicked() {
	m_ui->labelExample->setActive(false);
	m_ui->labelRecent->setActive(true);
	m_pageType = PT_RecentFiles;
	updateWelcomePage();
}


void SVWelcomeScreen::on_labelExampleClicked() {
	m_ui->labelExample->setActive(true);
	m_ui->labelRecent->setActive(false);
	m_pageType = PT_DemoFiles;
	updateWelcomePage();
}


void SVWelcomeScreen::onStyleChanged() {
	// set colors of clickable labels
	if (SVSettings::instance().m_theme == SVSettings::TT_Dark)
		setLabelColors("#eb9a2f");
	else
		setLabelColors("#eb9a2f");
	updateWelcomePage(); // this also updates html colors
	update();


	QPixmap pixmap = QIcon::fromTheme("simvicus_logo").pixmap(300);
	m_ui->labelLogo->setPixmap(pixmap);
}



const char * const HTML_TEMPLATE =
		"<html>\n"
		"<head>\n"
		"<style type=\"text/css\">\n"
		"body     { font-size: medium; color: ${STYLE_TEXT_COLOR}; background-color: ${STYLE_BACKGROUND_COLOR}; margin-left:0px }\n"
		"a        { color: ${STYLE_LINKTEXT_COLOR}; text-decoration:none; }\n"
		"a:hover  { color: ${STYLE_LINKTEXT_HOVER_COLOR}; background-color: ${STYLE_LINKTEXT_HOVER_BACKGROUND_COLOR}; }\n"
		"p        { font-size: medium; text-align: justify; margin-top:0px; margin-bottom:8px;   }\n"
		"h1       { font-size: large; color: ${STYLE_H1_COLOR}; font-weight:bold; \n"
		"           text-decoration: none; margin-top:15px; margin-bottom:15px }\n"
		"h2       { font-size: medium; color: ${STYLE_H2_COLOR}; font-weight:bold; margin-top:15px; margin-bottom:6px }\n"
		"h3       { font-size: medium; color: ${STYLE_H3_COLOR}; font-weight:bold; margin-top:10px; margin-bottom:2px}\n"
		"table    { font-size: medium }\n"
		"b        { color: black }\n"
		"pre      { font-size: small; font-family: monospace;courier }\n"
		"li       { text-align: justify }\n"
		"</style>\n"
		"\n"
		"</head>\n"
		"<body>\n"
		"${RECENT_PROJECT_FILELIST}\n"
		"</body>\n"
		"</html>\n"
		"\n";


const char * const HTML_NEWS_TEMPLATE =
		"<html>\n"
		"<head>\n"
		"<style type=\"text/css\">\n"
		"body     { font-size: medium; color: ${STYLE_TEXT_COLOR}; background-color: ${STYLE_BACKGROUND_COLOR} }\n"
		"a        { color: ${STYLE_LINKTEXT_COLOR}; text-decoration:none; }\n"
		"a:hover  { color: ${STYLE_LINKTEXT_HOVER_COLOR}; background-color: ${STYLE_LINKTEXT_HOVER_BACKGROUND_COLOR}; }\n"
		"p        { font-size: medium; text-align: justify; margin-top:0px; margin-bottom:8px;   }\n"
		"h1       { font-size: large; color: ${STYLE_H1_COLOR}; font-weight:bold; \n"
		"           text-decoration: none; margin-top:15px; margin-bottom:15px }\n"
		"h2       { font-size: medium; color: ${STYLE_H2_COLOR}; font-weight:bold; margin-top:15px; margin-bottom:6px }\n"
		"h3       { font-size: medium; color: ${STYLE_H3_COLOR}; font-weight:bold; margin-top:10px; margin-bottom:2px}\n"
		"table    { font-size: medium }\n"
		"b        { color: black }\n"
		"pre      { font-size: small; font-family: monospace;courier }\n"
		"li       { text-align: justify }\n"
		"</style>\n"
		"\n"
		"</head>\n"
		"<body>\n"
		"${NEWS_CONTENT}\n"
		"</body>\n"
		"</html>\n"
		"\n";


const char * const RECENT_PROJECT_TABLE_TEMPLATE =
		"<table border=\"0\" cellspacing=\"2\" cellpadding=\"0\">\n"
		"<tr valign=center><th width=\"${THUMBNAILSIZE}\" rowspan=\"3\">${IMG_FILENAME}</th><th align=left>${PROJECT_FILENAME}</th></tr>\n"
		"<tr valign=center><td align=left><a href=\"pfile:${PROJECT_FULL_PATH}\">${PROJECT_FULL_PATH}</a></td></tr>\n"
		"<tr valign=top><td align=justify>${PROJECT_DESCRIPTION}</td></tr>\n"
		"</table>\n"
		"<br>\n"
		"\n";

