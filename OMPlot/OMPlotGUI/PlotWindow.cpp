/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Linkoping University,
 * Department of Computer and Information Science,
 * SE-58183 Linkoping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3
 * AND THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S
 * ACCEPTANCE OF THE OSMC PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linkoping University, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main Authors 2010: Syed Adeel Asghar, Sonia Tariq
 *
 */

#include "PlotWindow.h"

using namespace OMPlot;

PlotWindow::PlotWindow(QStringList arguments, QWidget *parent)
    : QMainWindow(parent)
{
    // create an instance of qwt plot
    mpPlot = new Plot(this);
    // set up the toolbar
    setupToolbar();
    // open the file
    openFile(QString(arguments[1]));

    //Set up arguments
    // read the title
    setTitle(QString(arguments[2]));
    // read the legend
    if(QString(arguments[3]) == "true")
        setLegend(true);
    else if(QString(arguments[3]) == "false")
        setLegend(false);
    else
        throw PlotException("Invalid input");
    // read the grid
    if(QString(arguments[4]) == "true")
        setGrid(true);
    else if(QString(arguments[4]) == "false")
        setGrid(false);
    else
        throw PlotException("Invalid input");
    // read the plot type
    QString plotType = arguments[5];
    // read the logx
    if(QString(arguments[6]) == "true")
        setLogX(true);
    else if(QString(arguments[6]) == "false")
        setLogX(false);
    else
        throw PlotException("Invalid input");
    // read the logy
    if(QString(arguments[7]) == "true")
        setLogY(true);
    else if(QString(arguments[7]) == "false")
        setLogY(false);
    else
        throw PlotException("Invalid input");
    // read the x label value
    setXLabel(QString(arguments[8]));
    // read the y label value
    setYLabel(QString(arguments[9]));
    // read the x range
    setXRange(QString(arguments[10]).toDouble(), QString(arguments[11]).toDouble());
    // read the y range
    setYRange(QString(arguments[12]).toDouble(), QString(arguments[13]).toDouble());
    // read the variables name
    QStringList variablesToRead;
    for(int i = 14; i < arguments.length(); i++)
        variablesToRead.append(QString(arguments[i]));  

    //Plot
    if(plotType == "plot")
        plot(variablesToRead);
    if(plotType == "plotAll")
        plotAll();
    if(plotType == "plotParametric")
        plotParametric(QString(variablesToRead[0]), QString(variablesToRead[1]));

    setCentralWidget(mpPlot);
}

PlotWindow::PlotWindow(QString fileName, QWidget *parent)
        : QMainWindow(parent)
{
    // create an instance of qwt plot
    mpPlot = new Plot(this);
    // set up the toolbar
    setupToolbar();
    // open the file
    openFile(fileName);

    setCentralWidget(mpPlot);
}

// used for interactive simulation
PlotWindow::PlotWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // create an instance of qwt plot
    mpPlot = new Plot(this);
    // set up the toolbar
    setupToolbar();
    mpFile = 0;

    setCentralWidget(mpPlot);
}

PlotWindow::~PlotWindow()
{
    if (mpFile)
    {
        delete mpFile;
        delete mpTextStream;
    }
    delete mpPlot;
}

void PlotWindow::openFile(QString file)
{    
    //Open file to a textstream    
    mpFile = new QFile(file);
    if(!mpFile->exists())
        throw NoFileException(QString("File not found : ").append(file).toStdString().c_str());
    mpFile->open(QIODevice::ReadOnly);    
    mpTextStream = new QTextStream(mpFile);
}

void PlotWindow::setupToolbar()
{
    QToolBar *toolBar = new QToolBar(this);

    //ZOOM
    mpZoomButton = new QToolButton(toolBar);
    mpZoomButton->setText("Zoom");
    mpZoomButton->setCheckable(true);
    mpZoomButton->setChecked(true);
    connect(mpZoomButton, SIGNAL(toggled(bool)), SLOT(enableZoomMode(bool)));
    toolBar->addWidget(mpZoomButton);

    toolBar->addSeparator();

    //PAN
    mpPanButton = new QToolButton(toolBar);
    mpPanButton->setText("Pan");
    mpPanButton->setCheckable(true);
    connect(mpPanButton, SIGNAL(toggled(bool)), SLOT(enablePanMode(bool)));
    toolBar->addWidget(mpPanButton);

    toolBar->addSeparator();

    //ORIGINAL SIZE
    QToolButton *originalButton = new QToolButton(toolBar);
    originalButton->setText("Original");
    //btnExport->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(originalButton, SIGNAL(clicked()), SLOT(setOriginal()));
    toolBar->addWidget(originalButton);

    toolBar->addSeparator();

    //EXPORT
    QToolButton *btnExport = new QToolButton(toolBar);
    btnExport->setText("Export");
    //btnExport->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    connect(btnExport, SIGNAL(clicked()), SLOT(exportDocument()));
    toolBar->addWidget(btnExport);

    toolBar->addSeparator();

    //GRID
    mpGridButton = new QToolButton(toolBar);
    mpGridButton->setText("Grid");
    mpGridButton->setCheckable(true);
    mpGridButton->setChecked(true);
    connect(mpGridButton, SIGNAL(toggled(bool)), SLOT(setGrid(bool)));
    toolBar->addWidget(mpGridButton);

    toolBar->addSeparator();

    //LOG x LOG y
    mpXCheckBox = new QCheckBox("Log X", this);
    connect(mpXCheckBox, SIGNAL(clicked()), SLOT(setLog()));
    toolBar->addWidget(mpXCheckBox);
    toolBar->addSeparator();
    mpYCheckBox = new QCheckBox("Log Y", this);
    connect(mpYCheckBox, SIGNAL(clicked()), SLOT(setLog()));
    toolBar->addWidget(mpYCheckBox);

    //OPTIONS
//    QToolButton *pOptions = new QToolButton(toolBar);
//    pOptions->setText("Options");
//    pOptions->setCheckable(true);
//    connect(pOptions, SIGNAL(clicked()), ))
//    toolBar->addWidget(pOptions);

//    toolBar->addSeparator();

    addToolBar(toolBar);
}

void PlotWindow::plot(QStringList variables)
{
    QString currentLine;

    if(mpFile->fileName().endsWith("plt"))
    {
        //PLOT PLT
        //Set intervalSize
        int intervalSize;
        currentLine = mpTextStream->readLine();
        while(!currentLine.startsWith("#IntervalSize"))
            currentLine = mpTextStream->readLine();
        QStringList doubleLine = currentLine.split("=");
        QString intervalString = doubleLine[1];
        intervalSize = intervalString.toInt();

        bool plotAll = false;
        if(variables[0] == "")
            plotAll = true;        

        QVector<int> variableExists;
        for(int i = 0; i < variables.length(); i++)
            variableExists.push_back(0);

        //Assign values
        while(!mpTextStream->atEnd())
        {
            while(!currentLine.startsWith("DataSet:"))
            {
                currentLine = mpTextStream->readLine();
                if(mpTextStream->atEnd())
                    break;
            }

            QString currentVariable = currentLine.remove("DataSet: ");            

            for(int i = 0; i < variables.length(); i++)
            {                
                if(variables[i] == currentVariable || plotAll)
                {
                    variableExists.replace(i , 1);
                    PlotCurve *pPlotCurve = new PlotCurve(mpPlot);
                    currentLine = mpTextStream->readLine();
                    intervalSize = intervalString.toInt();
                    while(intervalSize != 0)
                    {
                        QStringList doubleList = currentLine.split(",");
                        pPlotCurve->addXAxisValue(QString(doubleList[0]).toDouble());
                        pPlotCurve->addYAxisValue(QString(doubleList[1]).toDouble());
                        currentLine = mpTextStream->readLine();
                        intervalSize--;
                    }
                    pPlotCurve->setTitle(currentVariable);
                    mpPlot->addPlotCurve(pPlotCurve);
                }
            }
        }
        //Error handling
        if(!(variables[0] == ""))        
            checkForErrors(variables, variableExists);
    }    
    else if(mpFile->fileName().endsWith("csv"))
    {
        //PLOT CSV
        currentLine = mpTextStream->readLine();
        currentLine.remove(QChar('"'));
        QStringList allVariablesInFile = currentLine.split(",");
        allVariablesInFile.removeLast();

        QVector<int> variableExists;
        for(int i = 0; i < variables.length(); i++)
            variableExists.push_back(0);

        //VariablesToPlotIndex = {0,2...}
        QVector<int> variablesToPlotIndex;
        for(int i = 0; i < variables.length(); i++)
        {
            for(int j = 0; j < allVariablesInFile.length(); j++)
            {
                if(variables[i] == allVariablesInFile[j])
                {
                    variablesToPlotIndex.push_back(j);
                    variableExists.replace(i , 1);
                }
            }
        }

        if(variables[0] == "")
        {
            for(int i = 0; i < allVariablesInFile.length(); i++)
                variablesToPlotIndex.push_back(i);
        }

        //Assign Values
        while(!mpTextStream->atEnd())
        {
            currentLine = mpTextStream->readLine();
            QStringList values = currentLine.split(",");
            values.removeLast();

            for(int i = 0; i < variablesToPlotIndex.size(); i++)
            {
                PlotCurve *pPlotCurve = new PlotCurve(mpPlot);
                QString valuesString = values[0];
                pPlotCurve->addXAxisValue(valuesString.toDouble());
                QString valuesString2 = values[variablesToPlotIndex[i]];
                pPlotCurve->addYAxisValue(valuesString2.toDouble());
                if(variables[0] == "")
                    pPlotCurve->setTitle(allVariablesInFile[i]);
                else
                    pPlotCurve->setTitle(variables[i]);
                mpPlot->addPlotCurve(pPlotCurve);
            }
        }

        //Error Handling
        if(!(variables[0] == ""))
            checkForErrors(variables, variableExists);
    }
    else if(mpFile->fileName().endsWith("mat"))
    {
        //PLOT MAT
        ModelicaMatReader reader;        
        ModelicaMatVariable_t *var;
        const char *msg = new char[20];

        //Read in mat file
        if(0 != (msg = omc_new_matlab4_reader(mpFile->fileName().toStdString().c_str(), &reader)))
            return;

        //Read in timevector
        QVector<double> timeVector;
        if (reader.nvar < 1)
          throw NoVariableException("Variable doesnt exist: time");
        double *vals = omc_matlab4_read_vals(&reader,0);
        for (int j = 0; j<reader.nrows; j++)
            timeVector.push_back(vals[j]);

        if(variables[0] == "")
        {
            for (int i = 0; i < reader.nall; i++)
                variables.append(reader.allInfo[i].name);
            variables.removeFirst();
        }

        //Read in values
        for(int i = 0; i < variables.length(); i++)
        {
            QString currentPlotVariable = variables[i];

            PlotCurve *pPlotCurve = new PlotCurve(mpPlot);
            pPlotCurve->setXAxisVector(timeVector);

            //Read in y vector variable
            var = omc_matlab4_find_var(&reader, currentPlotVariable.toStdString().c_str());
            if(!var)
                throw NoVariableException(QString("Variable doesnt exist : ").append(currentPlotVariable).toStdString().c_str());
            if (!var->isParam) {
              vals = omc_matlab4_read_vals(&reader, var->index);
              for (int j = 0; j < reader.nrows; j++)
                  pPlotCurve->addYAxisValue(vals[j]);
            } else {
              double val;
              if (omc_matlab4_val(&val,&reader,var,0.0))
                throw NoVariableException(QString("Parameter doesn't have a value : ").append(currentPlotVariable).toStdString().c_str());
              for (int j = 0; j < reader.nrows; j++)
                  pPlotCurve->addYAxisValue(val);
            }

            //Set curvename and push back to list
            pPlotCurve->setTitle(variables[i]);
            mpPlot->addPlotCurve(pPlotCurve);
        }
    }
    plotGraph(mpPlot->getPlotCurvesList());
}

void PlotWindow::plotAll()
{
    //PLOT PLT
    if(mpFile->fileName().endsWith("plt"))
        plot(QStringList(""));
    //PLOT CSV
    else if(mpFile->fileName().endsWith("csv"))
        plot(QStringList(""));
    //PLOT MAT
    else if(mpFile->fileName().endsWith("mat"))
        plot(QStringList(""));
}

void PlotWindow::plotParametric(QString xVariable, QString yVariable)
{
    QString currentLine;    
    QVector<int> variableExists(2);

    if(mpFile->fileName().endsWith("plt"))
    {
        //PLOT PLT
        //set intervalSize
        int intervalSize;
        currentLine = mpTextStream->readLine();
        while(!currentLine.startsWith("#IntervalSize"))
            currentLine = mpTextStream->readLine();
        QStringList doubleLine = currentLine.split("=");
        QString intervalString = doubleLine[1];
        intervalSize = intervalString.toInt();

        QStringList variablesList(xVariable);
        variablesList.append(yVariable);
        PlotCurve *pPlotCurve = new PlotCurve(mpPlot);

        //Collect variables
        while(!mpTextStream->atEnd())
        {
            while(!currentLine.startsWith("DataSet:"))
            {
                currentLine = mpTextStream->readLine();
                if(mpTextStream->atEnd())
                    break;
            }            

            QString currentVariable = currentLine.remove("DataSet: ");

            for(int i = 0; i < variablesList.length(); i++)
            {

                if(variablesList[i] == currentVariable)
                {
                    variableExists.replace(i , 1);
                    currentLine = mpTextStream->readLine();
                    intervalSize = intervalString.toInt();
                    while(intervalSize != 0)
                    {                       
                        QStringList doubleList = currentLine.split(",");
                        QString vS = doubleList[1];

                        if(i == 0)
                            pPlotCurve->addXAxisValue(vS.toDouble());
                        else if(i == 1)
                            pPlotCurve->addYAxisValue(vS.toDouble());

                        currentLine = mpTextStream->readLine();
                        intervalSize--;
                    }
                }
            }
        }

        //Error handling        
        checkForErrors(variablesList, variableExists);

        pPlotCurve->setTitle(variablesList[1]);
        mpPlot->addPlotCurve(pPlotCurve);
    }
    else if(mpFile->fileName().endsWith("csv"))
    {
        //PLOT CSV
        //VariablesLine = {x,y}
        currentLine = mpTextStream->readLine();
        currentLine.remove(QChar('"'));

        int xVariableIndex = 0;
        int yVariableIndex = 0;

        QStringList allVariables = currentLine.split(",");

        for(int j = 0; j < allVariables.length(); j++)
        {
            if(allVariables[j] == xVariable)
            {
                xVariableIndex = j;
                variableExists.replace(0 , 1);
            }
            if(allVariables[j] == yVariable)
            {
                yVariableIndex = j;
                variableExists.replace(1 , 1);
            }
        }

        currentLine = mpTextStream->readLine();
        PlotCurve *pPlotCurve = new PlotCurve(mpPlot);

        //Collect values
        while(!mpTextStream->atEnd())
        {
            QStringList values = currentLine.split(",");

            pPlotCurve->addXAxisValue(QString(values[xVariableIndex]).toDouble());
            pPlotCurve->addYAxisValue(QString(values[yVariableIndex]).toDouble());

            currentLine = mpTextStream->readLine();
        }
        //Error handling
        QStringList list(xVariable);
        list.append(yVariable);
        checkForErrors(list, variableExists);

        pPlotCurve->setTitle(yVariable);
        mpPlot->addPlotCurve(pPlotCurve);
    }
    else if(mpFile->fileName().endsWith("mat"))
    {
        //PLOT MAT
        //Declare variables
        ModelicaMatReader reader;        
        ModelicaMatVariable_t *var;
        const char *msg = new char[20];

        //Read the .mat file
        if(0 != (msg = omc_new_matlab4_reader(mpFile->fileName().toStdString().c_str(), &reader)))
            return;

        PlotCurve *pPlotCurve = new PlotCurve(mpPlot);

        //Fill variable x with data
        var = omc_matlab4_find_var(&reader, xVariable.toStdString().c_str());
        if(!var)
            throw NoVariableException(QString("Variable doesn't exist : ").append(xVariable).toStdString().c_str());
        double *vals = omc_matlab4_read_vals(&reader,var->index); //H�r sef fault om variabel fel
        for (int j = 0; j<reader.nrows; j++)
            pPlotCurve->addXAxisValue(vals[j]);

        //Fill variable y with data
        var = omc_matlab4_find_var(&reader, yVariable.toStdString().c_str());
        if(!var)
            throw NoVariableException(QString("Variable doesn't exist : ").append(yVariable).toStdString().c_str());
        vals = omc_matlab4_read_vals(&reader,var->index);
        for (int j = 0; j<reader.nrows; j++)
            pPlotCurve->addYAxisValue(vals[j]);

        pPlotCurve->setTitle(yVariable);
        mpPlot->addPlotCurve(pPlotCurve);
    }
    plotGraph(mpPlot->getPlotCurvesList());
}

void PlotWindow::plotGraph(QList<PlotCurve*> plotCurvesList)
{
    for(int i = 0; i < plotCurvesList.length(); i++)
    {
        int colorValue = 7 + i;
        if(colorValue > 19 )
            colorValue = 1 + i % 12;
        QPen pen(QColor(Qt::GlobalColor(colorValue + 1)));
        pen.setWidth(2);
        plotCurvesList[i]->setData(plotCurvesList[i]->getXAxisVector(), plotCurvesList[i]->getYAxisVector());
        plotCurvesList[i]->setPen(pen);
        plotCurvesList[i]->attach(mpPlot);
    }
}

void PlotWindow::enableZoomMode(bool on)
{
    mpPlot->getPlotZoomer()->setEnabled(on);
    if(on)
    {
        mpPlot->getPlotPicker()->setRubberBand(QwtPlotPicker::NoRubberBand);
        if(mpPlot->getPlotPanner()->isEnabled())
        {
            mpPlot->getPlotPanner()->setEnabled(false);
            mpPanButton->setChecked(false);
        }
        mpPlot->canvas()->setCursor(Qt::CrossCursor);
    }
    else
    {
        if(!mpPlot->getPlotPanner()->isEnabled())
            mpPlot->getPlotPicker()->setRubberBand(QwtPlotPicker::CrossRubberBand);
    }
}

void PlotWindow::enablePanMode(bool on)
{
    mpPlot->getPlotPanner()->setEnabled(on);

    if(on)
    {
        mpPlot->getPlotPicker()->setRubberBand(QwtPlotPicker::NoRubberBand);
        mpPlot->canvas()->setCursor(Qt::OpenHandCursor);
        if(mpPlot->getPlotZoomer()->isEnabled())
        {
            mpZoomButton->setChecked(false);
            enableZoomMode(false);
        }
    }
    else
    {
        if(!mpPlot->getPlotZoomer()->isEnabled())
        {
            mpPlot->getPlotPicker()->setRubberBand(QwtPlotPicker::CrossRubberBand);
            mpPlot->canvas()->setCursor(Qt::CrossCursor);
        }
    }
}

void PlotWindow::exportDocument()
{    
    //Include ps ;;Postscript Documents (*.ps)
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File As"), QDir::currentPath(), tr("Image Files (*.png *.bmp *.jpg)"));

    if ( !fileName.isEmpty() )
    {        
        QPixmap pixmap(mpPlot->size());
        mpPlot->render(&pixmap);
        pixmap.save(fileName);
    }
}

void PlotWindow::setLog()
{
    if(mpXCheckBox->isChecked())
    {
        mpPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
        mpPlot->setAxisAutoScale(QwtPlot::xBottom);
    }
    else if(mpYCheckBox->isChecked())
    {
        mpPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
        mpPlot->setAxisAutoScale(QwtPlot::yLeft);
    }
    else
    {
        mpPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
        mpPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
        mpPlot->setAxisAutoScale(QwtPlot::yLeft);
        mpPlot->setAxisAutoScale(QwtPlot::xBottom);
    }
    mpPlot->replot();
}

void PlotWindow::setGrid(bool on)
{
    if (!on)
    {
        mpPlot->getPlotGrid()->detach();
        mpGridButton->setChecked(false);
    }
    else
    {
        mpPlot->getPlotGrid()->attach(mpPlot);
        mpGridButton->setChecked(true);
    }
    mpPlot->replot();
}

void PlotWindow::setOriginal()
{
    mpPlot->setAxisAutoScale(QwtPlot::yLeft);
    mpPlot->setAxisAutoScale(QwtPlot::xBottom);
    mpPlot->replot();
}

void PlotWindow::setTitle(QString title)
{
    mpPlot->setTitle(QwtText(title));
}

void PlotWindow::setLegend(bool on)
{
    mpPlot->getLegend()->setVisible(on);
}

void PlotWindow::setLogX(bool on)
{
    if(on)
    {
        mpXCheckBox->setChecked(true);
        setLog();
    }
}

void PlotWindow::setLogY(bool on)
{
    if(on)
    {
        mpYCheckBox->setChecked(true);
        setLog();
    }
}

void PlotWindow::setXLabel(QString label)
{
    mpPlot->setAxisTitle(QwtPlot::xBottom, label);
}

void PlotWindow::setYLabel(QString label)
{
    mpPlot->setAxisTitle(QwtPlot::yLeft, label);
}

void PlotWindow::setXRange(double min, double max)
{
    if(!(max == 0 && min == 0))
        mpPlot->setAxisScale(QwtPlot::xBottom, min, max);
}

void PlotWindow::setYRange(double min, double max)
{
    if(!(max == 0 && min == 0))
        mpPlot->setAxisScale(QwtPlot::yLeft, min, max);
}

void PlotWindow::checkForErrors(QStringList variables, QVector<int> variableExists)
{
    QString nonExistingVariables = "";
    for(int i = 0; i < variableExists.size(); i++)
    {
        if(variableExists[i] == 0)
            nonExistingVariables.append(variables[i] + ",");
    }
    if(nonExistingVariables != "")
        throw NoVariableException(QString("Variable(s) doesnt exist : ").append(nonExistingVariables).toStdString().c_str());
}

Plot* PlotWindow::getPlot()
{
    return mpPlot;
}

QToolButton* PlotWindow::getPanButton()
{
    return mpPanButton;
}

//Options
//Options::Options(PlotWindow *pPlotWindow)
//{
//    setWindowTitle(QString("OMPLOT - Options"));
//    setAttribute(Qt::WA_DeleteOnClose);
//    setMaximumSize(175, 150);

//    mpPlotWindow = pPlotWindow;

//    setUpForm();
//}

//void Options::setUpForm()
//{
//    QGridLayout *label = new QGridLayout;
//    mpLabelGroup = new QGroupBox();
//    mpTitleLabel = new QLabel(tr("Title:"));
//    mpTitle = new QLineEdit("");
//    label->addWidget(mpTitleLabel, 0, 0);
//    label->addWidget(mpTitle, 0, 1);

//    mpLabelGroup->setLayout(label);

//    QGridLayout *mainLayout = new QGridLayout;
//    mainLayout->addWidget(mpLabelGroup, 1, 0);
//    setLayout(mainLayout);
//}

//void Options::edit()
//{
//    qDebug() << "Waxxuup";
//}