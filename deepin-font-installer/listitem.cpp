/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     rekols <rekols@foxmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "listitem.h"
#include "dsvgrenderer.h"
#include <QApplication>
#include <QPainter>
#include <QDebug>

ListItem::ListItem(DFontInfo *fontInfo, QPixmap *iconPixmap, QPixmap *closeNormalPixmap, QPixmap *closeHoverPixmap, QPixmap *closePressPixmap)
    : m_fontInfo(fontInfo),
      m_status(ListItem::None),
      m_iconPixmap(iconPixmap),
      m_closeNormalPixmap(closeNormalPixmap),
      m_closeHoverPixmap(closeHoverPixmap),
      m_closePressPixmap(closePressPixmap)
{
}

ListItem::~ListItem()
{
}

bool ListItem::sameAs(DSimpleListItem *item)
{
    return getFilePath() == (static_cast<ListItem*>(item))->getFilePath();
}

void ListItem::drawBackground(QRect rect, QPainter *painter, int index, bool isSelect)
{
    // draw line.
    painter->setOpacity(1);
    painter->setPen(QColor(151, 151, 151, 255 * 0.1));
    painter->drawLine(QPoint(50, rect.y() + rect.height() - 1),
                      QPoint(rect.width() - 10, rect.y() + rect.height() - 1));
}

void ListItem::drawHover(QRect rect, QPainter *painter)
{
    m_rect = rect;

    QPainterPath path;
    path.addRect(QRectF(rect));

    painter->setOpacity(1);
    painter->fillPath(path, QColor(0, 0, 0, 255 * 0.05));
}

void ListItem::setStatus(Status status)
{
    m_status = status;
}

void ListItem::drawForeground(QRect rect, QPainter *painter, int column, int index, bool isSelect)
{
    QFont font;
    font.setPointSize(11);
    font.setWeight(QFont::DemiBold);
    painter->setFont(font);
    painter->setPen(QPen(QColor("#000000")));
    painter->setOpacity(1);

    // draw icon.
    const int iconY = rect.y() + (rect.height() - m_iconPixmap->height() / m_iconPixmap->devicePixelRatio()) / 2;
    const int iconWidth = m_iconPixmap->width() / m_iconPixmap->devicePixelRatio();
    const int iconHeight = m_iconPixmap->height() / m_iconPixmap->devicePixelRatio();
    painter->drawPixmap(QRect(10, iconY, iconWidth, iconHeight), *m_iconPixmap);

    // draw name.
    QRect nameRect = rect;
    nameRect.setLeft(iconWidth + 15);
    nameRect.setHeight(rect.height() / 2);

    const QRectF nameBoundingRect = painter->boundingRect(nameRect, m_fontInfo->familyName, Qt::AlignLeft | Qt::AlignBottom);
    painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignBottom, m_fontInfo->familyName);

    // draw style name.
    const int styleX = nameBoundingRect.right() + 10;
    QRect styleRect = nameRect;
    styleRect.setLeft(styleX);
    styleRect.setRight(rect.right() - 80);
    styleRect.setHeight(styleRect.height() - 2);

    font.setPointSize(9);
    font.setWeight(QFont::Normal);
    painter->setFont(font);

    const QString styleStr = painter->fontMetrics().elidedText(m_fontInfo->styleName, Qt::ElideRight, styleRect.width());
    painter->drawText(styleRect, Qt::AlignLeft | Qt::AlignBottom, styleStr);

    // draw stauts.
    QRect statusRect = rect;
    QString statusStr = "";

    switch (m_status) {
    case Installed:
        painter->setPen(QColor("#528315"));
        statusStr = tr("Installed");
        break;
    case Installing:
        painter->setPen(QColor("#7C7C7C"));
        statusStr = tr("Installing");
        break;
    }

    const int statusWidth = painter->fontMetrics().width(statusStr);
    statusRect.setRight(rect.right() - 18);
    statusRect.setLeft(rect.right() - statusWidth - 18);
    font.setPointSize(11);
    painter->drawText(statusRect, Qt::AlignVCenter | Qt::AlignRight, statusStr);

    // draw description text.
    QRect descRect = rect;
    QString descStr = m_fontInfo->description;
    descRect.setLeft(iconWidth + 15);
    descRect.setTop(nameRect.bottom() + 2);

    if (m_fontInfo->sysVersion.isEmpty())
        m_fontInfo->sysVersion = tr("Unknown");

    const bool isInstalled = m_fontInfo->isInstalled;
    const bool isSampleVersion = m_fontInfo->sysVersion == m_fontInfo->version;

    if (isInstalled && m_status == ListItem::None) {
        if (isSampleVersion) {
            descStr = QString(tr("Same version installed"));
        } else {
            descStr = QString(tr("Other version installed: %1")).arg(m_fontInfo->sysVersion);
        }
        painter->setPen(QColor("#FF5A5A"));
    } else {
        descStr = m_fontInfo->description;
        painter->setPen(QColor("#5A5A5A"));
    }

    font.setPointSize(10);
    painter->setFont(font);
    descStr = painter->fontMetrics().elidedText(descStr, Qt::ElideRight, rect.width() - statusWidth - iconWidth - 50);
    painter->drawText(descRect, Qt::AlignLeft | Qt::AlignTop, descStr);

    if (m_status == Installed) {
        return;
    }

    if (m_close_button_status == Normal) {
        painter->drawPixmap(QRect(rect.width() - closeButtonPadding,
                                  rect.y() + (rect.height() - m_closeNormalPixmap->height()) / 2,
                                  m_closeNormalPixmap->width(),
                                  m_closeNormalPixmap->height()), *m_closeNormalPixmap);
    } else if (m_close_button_status == Hover) {
        painter->drawPixmap(QRect(rect.width() - closeButtonPadding,
                                  rect.y() + (rect.height() - m_closeHoverPixmap->height()) / 2,
                                  m_closeHoverPixmap->width(),
                                  m_closeHoverPixmap->height()), *m_closeHoverPixmap);
    } else if (m_close_button_status == Press) {
        painter->drawPixmap(QRect(rect.width() - closeButtonPadding,
                                  rect.y() + (rect.height() - m_closePressPixmap->height()) / 2,
                                  m_closePressPixmap->width(),
                                  m_closePressPixmap->height()), *m_closePressPixmap);
    }
}

QString ListItem::getFilePath()
{
    return m_fontInfo->filePath;
}

void ListItem::setCloseButtonStatus(CloseButtonStatus status)
{
    m_close_button_status = status;
}

bool ListItem::isHoverCloseButton(QPoint pos)
{
    // qDebug() << pos.x() << pos.y() << m_rect;

    return pos.x() > m_rect.width() - closeButtonPadding && pos.y() > (m_rect.height() - m_closeNormalPixmap->height()) / 2 && pos.y() < m_rect.height() - (m_rect.height() - m_closeNormalPixmap->height()) / 2;
}
