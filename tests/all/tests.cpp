/*
 * Copyright (C) 2008-2012 The QXmpp developers
 *
 * Authors:
 *  Jeremy Lainé
 *  Manjeet Dahiya
 *
 * Source:
 *  http://code.google.com/p/qxmpp
 *
 * This file is a part of QXmpp library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#include "QXmppDiscoveryIq.h"
#include "QXmppEntityTimeIq.h"
#include "QXmppNonSASLAuth.h"
#include "QXmppSessionIq.h"
#include "QXmppStreamFeatures.h"
#include "QXmppUtils.h"
#include "QXmppVersionIq.h"
#include "util.h"

class TestPackets : public QObject
{
    Q_OBJECT

private slots:
    void testDiscovery();
    void testDiscoveryWithForm();
    void testNonSaslAuth();
    void testSession();
    void testStreamFeatures();
    void testVersionGet();
    void testVersionResult();
    void testEntityTimeGet();
    void testEntityTimeResult();
};

void TestPackets::testDiscovery()
{
    const QByteArray xml(
        "<iq id=\"disco1\" from=\"benvolio@capulet.lit/230193\" type=\"result\">"
        "<query xmlns=\"http://jabber.org/protocol/disco#info\">"
        "<identity category=\"client\" name=\"Exodus 0.9.1\" type=\"pc\"/>"
        "<feature var=\"http://jabber.org/protocol/caps\"/>"
        "<feature var=\"http://jabber.org/protocol/disco#info\"/>"
        "<feature var=\"http://jabber.org/protocol/disco#items\"/>"
        "<feature var=\"http://jabber.org/protocol/muc\"/>"
        "</query>"
        "</iq>");

    QXmppDiscoveryIq disco;
    parsePacket(disco, xml);
    QCOMPARE(disco.verificationString(), QByteArray::fromBase64("QgayPKawpkPSDYmwT/WM94uAlu0="));
    serializePacket(disco, xml);
}

void TestPackets::testDiscoveryWithForm()
{
    const QByteArray xml(
        "<iq id=\"disco1\" to=\"juliet@capulet.lit/chamber\" from=\"benvolio@capulet.lit/230193\" type=\"result\">"
        "<query xmlns=\"http://jabber.org/protocol/disco#info\" node=\"http://psi-im.org#q07IKJEyjvHSyhy//CH0CxmKi8w=\">"
        "<identity xml:lang=\"en\" category=\"client\" name=\"Psi 0.11\" type=\"pc\"/>"
        "<identity xml:lang=\"el\" category=\"client\" name=\"Ψ 0.11\" type=\"pc\"/>"
        "<feature var=\"http://jabber.org/protocol/caps\"/>"
        "<feature var=\"http://jabber.org/protocol/disco#info\"/>"
        "<feature var=\"http://jabber.org/protocol/disco#items\"/>"
        "<feature var=\"http://jabber.org/protocol/muc\"/>"
        "<x xmlns=\"jabber:x:data\" type=\"result\">"
        "<field type=\"hidden\" var=\"FORM_TYPE\">"
        "<value>urn:xmpp:dataforms:softwareinfo</value>"
        "</field>"
        "<field type=\"text-multi\" var=\"ip_version\">"
        "<value>ipv4</value>"
        "<value>ipv6</value>"
        "</field>"
        "<field type=\"text-single\" var=\"os\">"
        "<value>Mac</value>"
        "</field>"
        "<field type=\"text-single\" var=\"os_version\">"
        "<value>10.5.1</value>"
        "</field>"
        "<field type=\"text-single\" var=\"software\">"
        "<value>Psi</value>"
        "</field>"
        "<field type=\"text-single\" var=\"software_version\">"
        "<value>0.11</value>"
        "</field>"
        "</x>"
        "</query>"
        "</iq>");

    QXmppDiscoveryIq disco;
    parsePacket(disco, xml);
    QCOMPARE(disco.verificationString(), QByteArray::fromBase64("q07IKJEyjvHSyhy//CH0CxmKi8w="));
    serializePacket(disco, xml);
}

void TestPackets::testNonSaslAuth()
{
    // Client Requests Authentication Fields from Server
    const QByteArray xml1(
        "<iq id=\"auth1\" to=\"shakespeare.lit\" type=\"get\">"
        "<query xmlns=\"jabber:iq:auth\"/>"
        "</iq>");

    QXmppNonSASLAuthIq iq1;
    parsePacket(iq1, xml1);
    serializePacket(iq1, xml1);

    // Client Provides Required Information (Plaintext)
    const QByteArray xml3(
        "<iq id=\"auth2\" type=\"set\">"
        "<query xmlns=\"jabber:iq:auth\">"
        "<username>bill</username>"
        "<password>Calli0pe</password>"
        "<resource>globe</resource>"
        "</query>"
        "</iq>");
    QXmppNonSASLAuthIq iq3;
    parsePacket(iq3, xml3);
    QCOMPARE(iq3.username(), QLatin1String("bill"));
    QCOMPARE(iq3.digest(), QByteArray());
    QCOMPARE(iq3.password(), QLatin1String("Calli0pe"));
    QCOMPARE(iq3.resource(), QLatin1String("globe"));
    serializePacket(iq3, xml3);

    // Client Provides Required Information (Plaintext)
    const QByteArray xml4(
        "<iq id=\"auth2\" type=\"set\">"
        "<query xmlns=\"jabber:iq:auth\">"
        "<username>bill</username>"
        "<digest>48fc78be9ec8f86d8ce1c39c320c97c21d62334d</digest>"
        "<resource>globe</resource>"
        "</query>"
        "</iq>");
    QXmppNonSASLAuthIq iq4;
    parsePacket(iq4, xml4);
    QCOMPARE(iq4.username(), QLatin1String("bill"));
    QCOMPARE(iq4.digest(), QByteArray("\x48\xfc\x78\xbe\x9e\xc8\xf8\x6d\x8c\xe1\xc3\x9c\x32\x0c\x97\xc2\x1d\x62\x33\x4d"));
    QCOMPARE(iq4.password(), QString());
    QCOMPARE(iq4.resource(), QLatin1String("globe"));
    serializePacket(iq4, xml4);
}

void TestPackets::testSession()
{
    const QByteArray xml(
        "<iq id=\"session_1\" to=\"example.com\" type=\"set\">"
        "<session xmlns=\"urn:ietf:params:xml:ns:xmpp-session\"/>"
        "</iq>");

    QXmppSessionIq session;
    parsePacket(session, xml);
    QCOMPARE(session.id(), QString("session_1"));
    QCOMPARE(session.to(), QString("example.com"));
    QCOMPARE(session.type(), QXmppIq::Set);
    serializePacket(session, xml);
}

void TestPackets::testStreamFeatures()
{
    const QByteArray xml("<stream:features/>");
    QXmppStreamFeatures features;
    parsePacket(features, xml);
    QCOMPARE(features.bindMode(), QXmppStreamFeatures::Disabled);
    QCOMPARE(features.sessionMode(), QXmppStreamFeatures::Disabled);
    QCOMPARE(features.nonSaslAuthMode(), QXmppStreamFeatures::Disabled);
    QCOMPARE(features.tlsMode(), QXmppStreamFeatures::Disabled);
    QCOMPARE(features.authMechanisms(), QStringList());
    QCOMPARE(features.compressionMethods(), QStringList());
    serializePacket(features, xml);

    const QByteArray xml2("<stream:features>"
        "<bind xmlns=\"urn:ietf:params:xml:ns:xmpp-bind\"/>"
        "<session xmlns=\"urn:ietf:params:xml:ns:xmpp-session\"/>"
        "<auth xmlns=\"http://jabber.org/features/iq-auth\"/>"
        "<starttls xmlns=\"urn:ietf:params:xml:ns:xmpp-tls\"/>"
        "<compression xmlns=\"http://jabber.org/features/compress\"><method>zlib</method></compression>"
        "<mechanisms xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\"><mechanism>PLAIN</mechanism></mechanisms>"
        "</stream:features>");
    QXmppStreamFeatures features2;
    parsePacket(features2, xml2);
    QCOMPARE(features2.bindMode(), QXmppStreamFeatures::Enabled);
    QCOMPARE(features2.sessionMode(), QXmppStreamFeatures::Enabled);
    QCOMPARE(features2.nonSaslAuthMode(), QXmppStreamFeatures::Enabled);
    QCOMPARE(features2.tlsMode(), QXmppStreamFeatures::Enabled);
    QCOMPARE(features2.authMechanisms(), QStringList() << "PLAIN");
    QCOMPARE(features2.compressionMethods(), QStringList() << "zlib");
    serializePacket(features2, xml2);
}

void TestPackets::testVersionGet()
{
    const QByteArray xmlGet(
    "<iq id=\"version_1\" to=\"juliet@capulet.com/balcony\" "
    "from=\"romeo@montague.net/orchard\" type=\"get\">"
    "<query xmlns=\"jabber:iq:version\"/></iq>");

    QXmppVersionIq verIqGet;
    parsePacket(verIqGet, xmlGet);
    QCOMPARE(verIqGet.id(), QLatin1String("version_1"));
    QCOMPARE(verIqGet.to(), QLatin1String("juliet@capulet.com/balcony"));
    QCOMPARE(verIqGet.from(), QLatin1String("romeo@montague.net/orchard"));
    QCOMPARE(verIqGet.type(), QXmppIq::Get);
    serializePacket(verIqGet, xmlGet);
}

void TestPackets::testVersionResult()
{
    const QByteArray xmlResult(
    "<iq id=\"version_1\" to=\"romeo@montague.net/orchard\" "
    "from=\"juliet@capulet.com/balcony\" type=\"result\">"
    "<query xmlns=\"jabber:iq:version\">"
        "<name>qxmpp</name>"
        "<os>Windows-XP</os>"
        "<version>0.2.0</version>"
    "</query></iq>");

    QXmppVersionIq verIqResult;
    parsePacket(verIqResult, xmlResult);
    QCOMPARE(verIqResult.id(), QLatin1String("version_1"));
    QCOMPARE(verIqResult.to(), QLatin1String("romeo@montague.net/orchard"));
    QCOMPARE(verIqResult.from(), QLatin1String("juliet@capulet.com/balcony"));
    QCOMPARE(verIqResult.type(), QXmppIq::Result);
    QCOMPARE(verIqResult.name(), QString("qxmpp"));
    QCOMPARE(verIqResult.version(), QString("0.2.0"));
    QCOMPARE(verIqResult.os(), QString("Windows-XP"));

    serializePacket(verIqResult, xmlResult);
}

void TestPackets::testEntityTimeGet()
{
    const QByteArray xml("<iq id=\"time_1\" "
        "to=\"juliet@capulet.com/balcony\" "
        "from=\"romeo@montague.net/orchard\" type=\"get\">"
      "<time xmlns=\"urn:xmpp:time\"/>"
    "</iq>");

    QXmppEntityTimeIq entityTime;
    parsePacket(entityTime, xml);
    QCOMPARE(entityTime.id(), QLatin1String("time_1"));
    QCOMPARE(entityTime.to(), QLatin1String("juliet@capulet.com/balcony"));
    QCOMPARE(entityTime.from(), QLatin1String("romeo@montague.net/orchard"));
    QCOMPARE(entityTime.type(), QXmppIq::Get);
    serializePacket(entityTime, xml);
}

void TestPackets::testEntityTimeResult()
{
    const QByteArray xml(
    "<iq id=\"time_1\" to=\"romeo@montague.net/orchard\" from=\"juliet@capulet.com/balcony\" type=\"result\">"
      "<time xmlns=\"urn:xmpp:time\">"
        "<tzo>-06:00</tzo>"
        "<utc>2006-12-19T17:58:35Z</utc>"
      "</time>"
    "</iq>");

    QXmppEntityTimeIq entityTime;
    parsePacket(entityTime, xml);
    QCOMPARE(entityTime.id(), QLatin1String("time_1"));
    QCOMPARE(entityTime.from(), QLatin1String("juliet@capulet.com/balcony"));
    QCOMPARE(entityTime.to(), QLatin1String("romeo@montague.net/orchard"));
    QCOMPARE(entityTime.type(), QXmppIq::Result);
    QCOMPARE(entityTime.tzo(), -21600);
    QCOMPARE(entityTime.utc(), QDateTime(QDate(2006, 12, 19), QTime(17, 58, 35), Qt::UTC));
    serializePacket(entityTime, xml);
}

QTEST_MAIN(TestPackets)
#include "tests.moc"