//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	Phonon Command Line Player				//
//  Edit:	18-Oct-18						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2018 Jonathan Marten <jjm@keelhaul.me.uk>		//
//  Home and download page:  http://www.keelhaul.me.uk/TBD/		//
//									//
//  This program is free software; you can redistribute it and/or	//
//  modify it under the terms of the GNU General Public License as	//
//  published by the Free Software Foundation; either version 3 of	//
//  the License, or (at your option) any later version.			//
//									//
//  It is distributed in the hope that it will be useful, but		//
//  WITHOUT ANY WARRANTY; without even the implied warranty of		//
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	//
//  GNU General Public License for more details.			//
//									//
//  You should have received a copy of the GNU General Public		//
//  License along with this program; see the file COPYING for further	//
//  details.  If not, see <http://www.gnu.org/licenses>.		//
//									//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <qcoreapplication.h>
#include <qcommandlineparser.h>
#include <qdebug.h>
#include <qurl.h>
#include <qdir.h>

#include <kaboutdata.h>
#include <klocalizedstring.h>
#include <kcrash.h>

#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/backendcapabilities.h>

//////////////////////////////////////////////////////////////////////////
//									//
//  Error reporting							//
//									//
//////////////////////////////////////////////////////////////////////////

static void errmsg(const QString &msg, const QString &severity = QString())
{
    QString sev = severity;
    if (sev.isEmpty()) sev = "error";

    std::cerr << qPrintable(QCoreApplication::instance()->applicationName())
              << " (" << qPrintable(sev.toUpper()) << "): "
              << qPrintable(msg) << std::endl;
}

static void errexit(const QString &msg, const QString &severity = QString())
{
    errmsg(msg, severity);
    exit(EXIT_FAILURE);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  canonicaliseDevice -- Canonicalise a Phonon device name by		//
//  removing any whitespace, even embedded.				//
//									//
//////////////////////////////////////////////////////////////////////////

static QString canonicaliseDevice(const QString &str)
{
    QString s = str.simplified();
    s.replace(QRegExp("\\s+"), "");
    return (s);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  parseFileArgument -- Parse and verify a media file (which can	//
//  actually be a URL) argument.					//
//									//
//  As recommended at							//
//  http://marc.info/?l=kde-core-devel&m=141359279227385&w=2		//
//									//
//////////////////////////////////////////////////////////////////////////

static QUrl parseFileArgument(const QString &arg)
{
    const QUrl u = QUrl::fromUserInput(arg, QDir::currentPath(), QUrl::AssumeLocalFile);
    if (!u.isValid()) errmsg(i18nc("@info:shell", "Invalid media URL '%1'", arg));
    return (u);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Main							 	//
//									//
//////////////////////////////////////////////////////////////////////////
 
int main(int argc, char *argv[])
{
    KAboutData aboutData("phononplay",			// componentName
                         i18n("Phonon Player"),		// displayName
                         VERSION,			// version
                         i18n("Command line Phonon audio player"),
                         KAboutLicense::GPL_V3,
                         i18n("Copyright (c) 2018 Jonathan Marten"),
                         "",				// otherText
                         "http://www.keelhaul.me.uk",	// homePageAddress
                        "jjm@keelhaul.me.uk");		// bugsEmailAddress
    aboutData.addAuthor(i18n("Jonathan Marten"),
                        "",
                        "jjm@keelhaul.me.uk",
                        "http://www.keelhaul.me.uk");

    QCoreApplication app(argc, argv);
    KAboutData::setApplicationData(aboutData);
    KCrash::setDrKonqiEnabled(true);

    QCommandLineParser parser;
    parser.setApplicationDescription(aboutData.shortDescription());

    parser.addOption(QCommandLineOption((QStringList() << "d" << "device"),
                                        i18n("Device name to output to"),
                                        i18n("device")));

    parser.addOption(QCommandLineOption((QStringList() << "l" << "list"),
                                        i18n("List the available output devices")));

    parser.addPositionalArgument("file", i18n("Media file or URL to play"), i18n("file..."));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
    const QStringList args = parser.positionalArguments();

    const QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();

    if (parser.isSet("list"))				// list devices mode
    {
        if (!parser.value("device").isEmpty()) errmsg(i18nc("@info:shell", "Option '--device' ignored for list mode ('--list')"), "warning");
        if (args.count()>0) errmsg(i18nc("@info:shell", "File arguments ignored for list mode ('--list')"), "warning");

        for (int i = 0; i<devices.count(); ++i)
        {
            const Phonon::AudioOutputDevice &dev = devices[i];
            const QString desc = canonicaliseDevice(dev.description());

            std::cout << qPrintable(QString("%1  %2").arg(i, -3).arg(desc)) << std::endl;
            std::cout << qPrintable(QString("     %1").arg(dev.name())) << std::endl;
        }

        return (EXIT_SUCCESS);
    }

    if (args.count()==0)				// must have some files
    {
        errexit(i18nc("@info:shell", "No media arguments specified (use '--help' for help)"));
    }

    QString audioDevice = canonicaliseDevice(parser.value("device"));
    int deviceIndex = -1;
    if (!audioDevice.isEmpty())				// audio device is specified
    {
        bool ok;
        deviceIndex = audioDevice.toUInt(&ok);		// first try as an index number
        if (!ok)					// failed to convert, try as a name
        {
            deviceIndex = -1;
            for (int i = 0; i<devices.count(); ++i)
            {
                const Phonon::AudioOutputDevice &dev = devices[i];
                const QString desc = canonicaliseDevice(dev.description());
                if (desc==audioDevice)			// found device with matching name
                {
                    deviceIndex = i;
                    break;
                }
            }
        }

        if (deviceIndex==-1)
        {
            errexit(i18nc("@info:shell", "Output device '%1' not known, use '--list' to list", audioDevice));
        }

        if (deviceIndex>=devices.count())
        {
            errexit(i18nc("@info:shell", "Output device index %1 invalid, use '--list' to list", deviceIndex));
        }
    }

    Phonon::MediaObject *media = new Phonon::MediaObject;
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory);

    if (deviceIndex!=-1)				// device was explicitly specified
    {
        const Phonon::AudioOutputDevice &dev = devices[deviceIndex];
        if (!audioOutput->setOutputDevice(dev))		// set the output device
        {
            errexit(i18nc("@info:shell", "Failed to set the audio output device"));
        }
    }

    Phonon::createPath(media, audioOutput);

    const Phonon::AudioOutputDevice &actualDevice = audioOutput->outputDevice();
    errmsg(i18nc("@info:shell", "Using audio device '%1'", canonicaliseDevice(actualDevice.description())), "info");

    int i = 0;						// argument index

    QObject::connect(media, &Phonon::MediaObject::currentSourceChanged,
                     [&](const Phonon::MediaSource &src)
                     {
                         errmsg(i18nc("@info:shell", "Playing media '%1'", src.url().toDisplayString()), "info");
                     });

    QObject::connect(media, &Phonon::MediaObject::aboutToFinish,
                     [&]()
                     {
                         while (1)			// until valid media queued
                         {
                             ++i;
                             if (i>=args.count())	// end of command arguments
                             {
                                 errmsg(i18nc("@info:shell", "Finished"), "success");
                                 QCoreApplication::exit(EXIT_SUCCESS);
                                 return;		// from lambda, then app.exec()
                             }

                             const QUrl u = parseFileArgument(args[i]);
                             if (!u.isValid()) continue;
                             media->enqueue(u);
                             break;
                         }
                     });

    const QUrl u = parseFileArgument(args[0]);		// first media argument
    if (!u.isValid()) return (EXIT_FAILURE);		// assume must be valid
    media->enqueue(u);

    media->play();
    return (app.exec());
}
