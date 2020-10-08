//=============================================================================================================
/**
 * @file     analyzedatamodel.cpp
 * @author   Lorenz Esch <lesch@mgh.harvard.edu>
 * @since    0.1.2
 * @date     May, 2019
 *
 * @section  LICENSE
 *
 * Copyright (C) 2019, Lorenz Esch. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 * the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 *       following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 *       the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
 *       to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @brief    Definition of the Analyze Data Model Class.
 *
 */

//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "bidsviewmodel.h"
#include <iostream>

//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QtDebug>

//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace DISPLIB;

//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

BidsViewModel::BidsViewModel(QObject *pParent)
: QStandardItemModel(pParent)
{

}

//=============================================================================================================

BidsViewModel::~BidsViewModel()
{
}

//=============================================================================================================

void BidsViewModel::addData(QModelIndex selectedItem,
                            QStandardItem* pNewItem)
{
    qDebug() << selectedItem;

    if(!selectedItem.isValid()) {
        addMegDataToSession(addSessionToSubject(addSubject("sub-01"), "ses-01"), pNewItem);
    } else {
        switch(itemFromIndex(selectedItem)->data(ITEM_TYPE).value<int>()){
            case SUBJECT:
                qDebug() << "[BidsViewModel::addData] Prompt user to select a session";
                break;
            default:
                addMegDataToSession(itemFromIndex(selectedItem)->data(ITEM_SESSION).value<QModelIndex>(), pNewItem);
                break;
        }
    }
}

//=============================================================================================================

void BidsViewModel::addAvgToData(QStandardItem *pNewAvgItem,
                                  const QModelIndex &parentIndex)
{
    QStandardItem* selectedData = itemFromIndex(parentIndex);
    selectedData->setChild(selectedData->rowCount(),
                           pNewAvgItem);

    pNewAvgItem->setData(QVariant::fromValue(AVG), ITEM_TYPE);
    pNewAvgItem->setData(itemFromIndex(parentIndex)->data(ITEM_SUBJECT), ITEM_SUBJECT);
    pNewAvgItem->setData(itemFromIndex(parentIndex)->data(ITEM_SESSION), ITEM_SESSION);

    emit newItemIndex(pNewAvgItem->index());
}
//=============================================================================================================

QModelIndex BidsViewModel::addSubject(const QString &sSubjectName)
{
    //Ensure subject name follow BIDS format
    QString sNewSubjectName;

    if(!sSubjectName.startsWith("sub-")){
        sNewSubjectName = "sub-" + sSubjectName;
    } else {
        sNewSubjectName = sSubjectName;
    }

    sNewSubjectName.remove(" ");

    //Add subject to model
    QStandardItem* pSubjectItem = new QStandardItem(sNewSubjectName);
    appendRow(pSubjectItem);

    pSubjectItem->setData(QVariant::fromValue(SUBJECT), ITEM_TYPE);
    pSubjectItem->setData(QVariant::fromValue(pSubjectItem->index()), ITEM_SUBJECT);

    emit newItemIndex(pSubjectItem->index());

    return pSubjectItem->index();
}

//=============================================================================================================

QModelIndex BidsViewModel::addSessionToSubject(const QString &sSubjectName,
                                        const QString &sSessionName)
{
    //Ensure session name follow BIDS format
    QString sNewSessionName;

    if(!sSessionName.startsWith("ses-")){
        sNewSessionName = "ses-" + sSessionName;
    } else {
        sNewSessionName = sSessionName;
    }

    sNewSessionName.remove(" ");

    QList<QStandardItem*> listItems = findItems(sSubjectName);
    bool bRepeat = false;

    //Check for repeated names, no names
    if (listItems.size() > 1) {
        qWarning() << "[BidsViewModel::addSessionToSubject] Multiple subjects with same name";
        bRepeat = true;
    } else if (listItems.size() == 0) {
        qWarning() << "[BidsViewModel::addSessionToSubject] No Subject found with name:" << sSubjectName;
        return QModelIndex();
    }

    QStandardItem* pNewSessionItem;

    //Add session to subjects with mathcing names. Renames them if multiple.
    for (int i = 0; i < listItems.size(); i++){
        QStandardItem* pSubjectItem = listItems.at(i);

        if(bRepeat){
            pSubjectItem->setText(pSubjectItem->text() + QString::number(i + 1));
        }

        pNewSessionItem = new QStandardItem(sNewSessionName);
        pSubjectItem->setChild(pSubjectItem->rowCount(),
                               pNewSessionItem);

        pNewSessionItem->setData(QVariant::fromValue(SESSION), ITEM_TYPE);
        pNewSessionItem->setData(QVariant::fromValue(pSubjectItem->index()), ITEM_SUBJECT);
        pNewSessionItem->setData(QVariant::fromValue(pNewSessionItem->index()), ITEM_SESSION);

        emit newItemIndex(pNewSessionItem->index());
    }

    return pNewSessionItem->index();
}

//=============================================================================================================

QModelIndex BidsViewModel::addSessionToSubject(QModelIndex subjectIndex,
                                        const QString &sSessionName)
{
    //Ensure session name follow BIDS format
    QString sNewSessionName;

    if(!sSessionName.startsWith("ses-")){
        sNewSessionName = "ses-" + sSessionName;
    } else {
        sNewSessionName = sSessionName;
    }

    sNewSessionName.remove(" ");

    QStandardItem* pSubjectItem = itemFromIndex(subjectIndex);
    QStandardItem* pNewSessionItem = new QStandardItem(sNewSessionName);
    pSubjectItem->setChild(pSubjectItem->rowCount(),
                           pNewSessionItem);

    pNewSessionItem->setData(QVariant::fromValue(SESSION), ITEM_TYPE);
    pNewSessionItem->setData(QVariant::fromValue(subjectIndex), ITEM_SUBJECT);
    pNewSessionItem->setData(QVariant::fromValue(pNewSessionItem->index()), ITEM_SESSION);

    emit newItemIndex(pNewSessionItem->index());

    return pNewSessionItem->index();
}

//=============================================================================================================

QModelIndex BidsViewModel::addMegDataToSession(QModelIndex sessionIndex,
                                               QStandardItem *pNewItem)
{
    QStandardItem* pSessionItem = itemFromIndex(sessionIndex);
    bool bMegFolder = false;
    int iMegFolder = 0;

    for(iMegFolder; iMegFolder < pSessionItem->rowCount(); iMegFolder++){
        if (pSessionItem->child(iMegFolder)->text() == "meg"){
            bMegFolder = true;
            break;
        }
    }

    if(!bMegFolder) {
        QStandardItem* pMEGItem = new QStandardItem("meg");
        pMEGItem->setData(QVariant::fromValue(FOLDER), ITEM_TYPE);
        pMEGItem->setData(QVariant::fromValue(sessionIndex), ITEM_SESSION);
        pMEGItem->setData(itemFromIndex(sessionIndex)->data(ITEM_SUBJECT), ITEM_SUBJECT);

        pSessionItem->setChild(pSessionItem->rowCount(),
                               pMEGItem);
        pMEGItem->setChild(pMEGItem->rowCount(),
                           pNewItem);
    } else {
        pSessionItem->child(iMegFolder)->setChild(pSessionItem->child(iMegFolder)->rowCount(),
                                                  pNewItem);
    }

    pNewItem->setData(QVariant::fromValue(MEGDATA), ITEM_TYPE);
    pNewItem->setData(QVariant::fromValue(sessionIndex), ITEM_SESSION);
    pNewItem->setData(itemFromIndex(sessionIndex)->data(ITEM_SUBJECT), ITEM_SUBJECT);

    emit newItemIndex(pNewItem->index());

    return pNewItem->index();
}

//=============================================================================================================

QModelIndex BidsViewModel::moveSessionToSubject(QModelIndex subjectIndex,
                                                QModelIndex sessionIndex)
{
    beginResetModel();

    QStandardItem* subjectItem = itemFromIndex(subjectIndex);
    QStandardItem* sessionItem = itemFromIndex(sessionIndex);

    sessionItem->parent()->takeRow(sessionItem->row());


    subjectItem->setChild(subjectItem->rowCount(), sessionItem);
    subjectItem->setData(subjectIndex, ITEM_SUBJECT);

    for (int i = 0; i < sessionItem->rowCount(); i++){
        sessionItem->child(i)->setData(subjectIndex, ITEM_SUBJECT);
        for (int j = 0; j < sessionItem->child(i)->rowCount(); j++) {
            sessionItem->child(i)->child(j)->setData(subjectIndex, ITEM_SUBJECT);
        }
    }

    endResetModel();

    emit newItemIndex(sessionItem->index());

    return sessionItem->index();
}

//=============================================================================================================

QModelIndex BidsViewModel::moveDataToSession(QModelIndex sessionIndex,
                                             QModelIndex dataIndex)
{
    beginResetModel();

    QStandardItem* dataItem = itemFromIndex(dataIndex);

    if(dataItem->parent()->rowCount() < 2){
        QStandardItem* parent = dataItem->parent();

        dataItem->parent()->takeRow(dataItem->row()).first();
        parent->parent()->takeRow(parent->row()).first();
    } else {
        dataItem->parent()->takeRow(dataItem->row()).first();
    }

    QModelIndex newIndex = addMegDataToSession(sessionIndex,
                                               dataItem);

    endResetModel();

    emit newItemIndex(sessionIndex);
    emit newItemIndex(newIndex);

    return newIndex;
}
