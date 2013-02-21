#include "templates.h"
#include "core/project.h"
#include "tools.h"

Templates::Templates(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *gridLayout;

    // Left Widget
    QWidget *left = new QWidget;
    gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(0, 0, 0, 0);
    left->setLayout(gridLayout);

    m_list = new QListWidget;
    connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
    gridLayout->addWidget(m_list, 1, 0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    gridLayout->addLayout(buttonsLayout, 2, 0);

    QPushButton *addButton = new QPushButton(tr("Add"));
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAddButtonClicked()));
    buttonsLayout->addWidget(addButton);

    QPushButton *removeButton = new QPushButton(tr("Remove"));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(onRemoveButtonClicked()));
    buttonsLayout->addWidget(removeButton);

    // Right Widget
    QWidget *right = new QWidget;
    gridLayout = new QGridLayout();
    right->setLayout(gridLayout);

    m_nameWidget = new QLineEdit();
    m_descriptionWidget = new QLineEdit();
    m_headerWidget = new QLineEdit();
    m_footerWidget = new QLineEdit();
    m_baseWidget = new QLineEdit();
    connectWidgets();

    QPushButton *headerSearchButton = new QPushButton(tr("Search..."));
    headerSearchButton->setProperty("lineedit", qVariantFromValue((QObject*)m_headerWidget));
    connect(headerSearchButton, SIGNAL(clicked()), this, SLOT(onSearchButtonClicked()));
    QPushButton *footerSearchButton = new QPushButton(tr("Search..."));
    footerSearchButton->setProperty("lineedit", qVariantFromValue((QObject*)m_footerWidget));
    connect(footerSearchButton, SIGNAL(clicked()), this, SLOT(onSearchButtonClicked()));
    QPushButton *templateSearchButton = new QPushButton(tr("Search..."));
    templateSearchButton->setProperty("lineedit", qVariantFromValue((QObject*)m_baseWidget));
    connect(templateSearchButton, SIGNAL(clicked()), this, SLOT(onSearchButtonClicked()));

    int row = 0;
    gridLayout->addWidget(new QLabel(tr("Name:")), row, 0);
    gridLayout->addWidget(m_nameWidget, row++, 1, 1, 2);
    gridLayout->addWidget(new QLabel(tr("Description:")), row, 0);
    gridLayout->addWidget(m_descriptionWidget, row++, 1, 1, 2);
    gridLayout->addWidget(new QLabel(tr("Page Header:")), row, 0);
    gridLayout->addWidget(m_headerWidget, row, 1);
    gridLayout->addWidget(headerSearchButton, row++, 2);
    gridLayout->addWidget(new QLabel(tr("Page Footer:")), row, 0);
    gridLayout->addWidget(m_footerWidget, row, 1);
    gridLayout->addWidget(footerSearchButton, row++, 2);
    gridLayout->addWidget(new QLabel(tr("Template base:")), row, 0);
    gridLayout->addWidget(m_baseWidget, row, 1);
    gridLayout->addWidget(templateSearchButton, row++, 2);
    gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row++, 0, 1, 2);

    // Splitter
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QSplitter *hSplitter = new QSplitter(Qt::Horizontal);
    layout->addWidget(hSplitter);

    hSplitter->addWidget(left);
    hSplitter->addWidget(right);
    hSplitter->setStretchFactor(0, 30);
    hSplitter->setStretchFactor(1, 70);

    m_currentTemplate = NULL;

    connect(g_project, SIGNAL(projectLoad()), this, SLOT(onProjectLoad()));
}

void Templates::print(Proposal *proposal)
{
    m_currentTemplate = g_project->getTemplate(proposal->getString("template"));
    if(!m_currentTemplate) {
        qCritical() << tr("Failed loading template.");
        return;
    }

    if(m_currentTemplate->getString("base").isEmpty()) {
        qCritical() << tr("Templase base must be set.");
        return;
    }

    QString filename = proposal->getString("reference");
    filename = filename.toLower();
    filename = filename.replace(" ", "_");

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);

    QPrintPreviewDialog preview(&printer);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
}

void Templates::connectWidgets()
{
    connect(m_nameWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_descriptionWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_headerWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_footerWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    connect(m_baseWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
}

void Templates::disconnectWidgets()
{
    disconnect(m_nameWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_descriptionWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_headerWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_footerWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
    disconnect(m_baseWidget, SIGNAL(textChanged(QString)), this, SLOT(onWidgetChanged()));
}

void Templates::updateList()
{
    QString currentName;
    if(m_list->currentItem())
        currentName = m_list->currentItem()->text();

    QListWidgetItem *currentItem = NULL;

    disconnect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
    m_list->clear();
    const QVector<Template*>& tps = g_project->getTemplates();
    for(QVector<Template*>::const_iterator it = tps.constBegin(), end = tps.constEnd(); it != end; ++it) {
        QListWidgetItem *item = new QListWidgetItem((*it)->getString("name"));
        m_list->addItem(item);

        if((*it)->getString("name") == currentName)
            currentItem = item;
    }
    m_list->sortItems();
    connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));

    if(currentItem)
        m_list->setCurrentItem(currentItem);
    else if(m_list->count() > 0)
        m_list->setCurrentRow(0);
}

void Templates::updateTemplate(const QString& name)
{
    Template *tp = g_project->getTemplate(name);
    if(tp) {
        tp->set("name", m_nameWidget->text());
        tp->set("description", m_descriptionWidget->text());
        tp->set("header", m_headerWidget->text());
        tp->set("footer", m_footerWidget->text());
        tp->set("base", m_baseWidget->text());
    }
}

void Templates::onWidgetChanged()
{
    if(m_list->currentItem()) {
        QString name = m_list->currentItem()->text();
        updateTemplate(name);

        if(sender() && sender() == m_nameWidget) {
            disconnect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
            m_list->currentItem()->setText(m_nameWidget->text());
            m_list->sortItems();
            connect(m_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onListItemChanged(QListWidgetItem*,QListWidgetItem*)));
        }
    }
    else {
        qCritical() << tr("You need to create a template before editing.");
    }
}

void Templates::onListItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(previous) {
        QString name = previous->text();
        updateTemplate(name);
    }
    if(current) {
        QString name = current->text();
        Template *tp = g_project->getTemplate(name);
        disconnectWidgets();
        m_nameWidget->setText(tp->getString("name"));
        m_descriptionWidget->setText(tp->getString("description"));
        m_headerWidget->setText(tp->getString("header"));
        m_footerWidget->setText(tp->getString("footer"));
        m_baseWidget->setText(tp->getString("base"));
        connectWidgets();
    }
}

void Templates::onAddButtonClicked()
{
    QStringList a = Tools::requestDataFromUser(tr("Add Template"), tr("Name,"
                                                                    "Description").split(","));
    if(!Tools::isRequestedDataValid(a))
        return;

    Template *tp = new Template;
    tp->set("name", a[0]);
    tp->set("description", a[1]);

    if(g_project->addTemplate(tp))
        updateList();
    else
        qCritical() << tr("A template with this name already exists.");
}

void Templates::onRemoveButtonClicked()
{
    QListWidgetItem *currentItem = m_list->currentItem();
    if(currentItem) {
        if(Tools::requestYesNoFromUser(tr("Remove Template"), tr("Do you really want to remove this template?")) == "No")
            return;

        g_project->removeTemplate(currentItem->text());
        m_list->takeItem(m_list->row(currentItem));
        delete currentItem;
    }
}

void Templates::onSearchButtonClicked()
{
    QObject *object = sender();
    if(!object)
        return;

    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(object->property("lineedit").value<QObject*>());
    if(!lineEdit)
        return;

    QString dir = QFileInfo(g_project->getFilename()).absoluteFilePath() + "*.html";
    QString fileExtension = tr("Template file (*.html)");

    QString filename = QFileDialog::getOpenFileName(this, tr("Open"), dir, fileExtension);
    if(!filename.isEmpty())
        lineEdit->setText(filename);
}

void Templates::printPreview(QPrinter *printer)
{
    QSizeF pageSize = printer->pageRect().size();

    // Header
    QTextDocument header;
    QSizeF headerSize(0, 0);
    if(!m_currentTemplate->getString("header").isEmpty()) {
        QFile headerFile(m_currentTemplate->getString("header"));
        if(!headerFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QByteArray headerContent = headerFile.readAll();
        headerFile.close();

        header.setPageSize(QSize(printer->pageRect().size().width(), -1));
        header.setHtml(headerContent);
        headerSize = header.documentLayout()->documentSize();
    }

    // Footer
    QTextDocument footer;
    QSizeF footerSize(0, 0);
    if(!m_currentTemplate->getString("footer").isEmpty()) {
        QFile footerFile(m_currentTemplate->getString("footer"));
        if(!footerFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QByteArray footerContent = footerFile.readAll();
        footerFile.close();

        QTextDocument footer;
        footer.setPageSize(QSize(printer->pageRect().size().width(), -1));
        footer.setHtml(footerContent);
        footerSize = footer.documentLayout()->documentSize();
    }

    // Base
    QFile baseFile(m_currentTemplate->getString("base"));
    if(!baseFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QByteArray baseContent = baseFile.readAll();
    baseFile.close();

    QTextDocument base;
    base.setPageSize(pageSize - QSize(0, headerSize.height()));
    base.setHtml(baseContent);

    // Begin Painting
    QPainter painter;
    if(!painter.begin(printer)) {
        qCritical("Failed to open file, is it writable?");
        return;
    }
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    int pages = 1;
    qreal maxHeight = pageSize.height() - (headerSize.height() + footerSize.height());
    QRectF baseDrawRect = QRectF(QPointF(0, 0), QSizeF(pageSize.width(), maxHeight));
    do {
        painter.save();

        if(!header.isEmpty()) {
            header.drawContents(&painter);
            painter.translate(0, headerSize.height());
        }

        painter.save();
        painter.translate(0, -baseDrawRect.y());
        base.drawContents(&painter, baseDrawRect);
        baseDrawRect.translate(0, baseDrawRect.height());
        painter.restore();

        if(!footer.isEmpty()) {
            footer.drawContents(&painter);
            painter.translate(0, footerSize.height());
        }

        painter.restore();

        if(pages >= base.pageCount())
            break;
        pages++;

        if(!printer->newPage()) {
            qWarning() << tr("Failed in flushing page to disk, disk full?");
            return;
        }
    } while(true);

    // End Painting
    painter.end();
}

void Templates::onProjectLoad()
{
    updateList();
}
