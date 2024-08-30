//
// Created by snownf on 24-8-22.
//

#ifndef CONTROL_PANEL_XMLHELPER_H
#define CONTROL_PANEL_XMLHELPER_H

#include <QDomElement>

class XmlHelper {
    QDomDocument doc;
public:
    bool openFile(const QString &filePath);

    bool writeToFile(const QString &filePath);

    void replaceElement(QDomElement &element, const QString &newValue);

    QDomElement getOrCreateElement(QDomElement &parent, const QString &elementName);

    QDomElement getRoot();
};


#endif //CONTROL_PANEL_XMLHELPER_H
