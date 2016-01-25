
#ifndef PAGES_H
#define PAGES_H

#include <QWidget>

class ConfigurationPage : public QWidget
{
public:
    ConfigurationPage(QWidget *parent = 0);
    virtual ~ConfigurationPage(){}
};

class UpdatePage : public QWidget
{
public:
    UpdatePage(QWidget *parent = 0);
};

#endif
