//
// Created by snownf on 24-8-22.
//

#include <QFile>
#include <iostream>
#include <QTextStream>
#include "XmlHelper.h"

bool XmlHelper::openFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Failed to open file:" << filePath.toStdString();
        return false;
    }
    if (!doc.setContent(&file)) {
        std::cerr << "Failed to parse XML content.";
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool XmlHelper::writeToFile(const QString &filePath) {
    QFile outFile(filePath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        std::cerr << "Failed to open file for writing:" << filePath.toStdString();
        return false;
    }
    QTextStream stream(&outFile);
    stream << doc.toString();
    outFile.close();
    return true;
}

void XmlHelper::replaceElement(QDomElement &element, const QString &newValue) {
    QDomText matrixText = doc.createTextNode(newValue);
    element.removeChild(element.firstChild());
    element.appendChild(matrixText);
}

QDomElement XmlHelper::getOrCreateElement(QDomElement &parent, const QString &elementName) {
    QDomElement element = parent.firstChildElement(elementName);
    if (element.isNull()) {
        element = doc.createElement(elementName);
        parent.appendChild(element);
    }
    return element;
}

QDomElement XmlHelper::getRoot() {
    return doc.documentElement();
}