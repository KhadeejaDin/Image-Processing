// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// HistogramStretching.cpp - Histogram Stretching widget.
//
// Written by: Khadeeja Din, 2016
// ======================================================================


#include "MainWindow.h"
#include "HistogramStretching.h"

extern MainWindow *g_mainWindowP;



// Min and Max values for sliders
// initializing Histogram array of 256 entries
int Min = 0;
int Max = MaxGray;
int Histogram[MXGRAY];



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramStretching::HistogramStretching:
//
// Constructor.
//
HistogramStretching::HistogramStretching(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramStretching::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
HistogramStretching::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if (I1.isNull()) return 0;

	// initializing min and max values for finding the min and max pixel intensity in the input image
	int min = 0;
	int max = MaxGray;

	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// initializing Histogram with all 0 entries
	int i = 0;
	for (i = 0; i<MXGRAY; i++)
	 {Histogram[i] = 0;}

	 // reading input pixels values and storing their frequencies in Histogram
	 // p1 is a pointer that points to current pixel in I1. p1++ is pointing to next pixel in I1
   // p2 is a pointer that points to current pixel in I2. p2++ is pointing to next pixel in I2
	 int type;
	 ChannelPtr<uchar> p1, p2, endd;
	 for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		for(endd = p1 + total; p1<endd; p1++)
	  Histogram[*p1] ++;
	 }

  // check if auto checkboxes are checked
  int minautovalue = m_checkBoxMin->isChecked();
  int maxautovalue = m_checkBoxMax->isChecked();

	// minimum and maximum histogram stretch variables
  int minstretch, maxstretch;

	// get min and max values from siders
	minstretch = m_sliderMin->value();
	maxstretch = m_sliderMax->value();

	// if minautovalue is checked then read minimum pixel intensity from image
	// reading first non zero value from left of image and copying it to minimum
	// change values for slider and spinbox to minimum value of image
	if (minautovalue)
	{
		int minimum = min, i =0;
		for (i=0; i<MXGRAY; i++)
		{ if (!Histogram[i]) continue;
				minimum = i;
				break; }
		minstretch = minimum;
		m_sliderMin->setValue (minimum);
		m_spinBoxMin->setValue (minimum);
	}

	// if maxautovalue is checked then read maximum pixel intensity from image
	// reading first non zero value from right of image and copying it to maximum
	// change values for slider and spinbox to maximum value of image
	if (maxautovalue)
	{
		int maximum = MaxGray;
		int i = 0;
		for (i=MaxGray; i>= 0; i--)
		{ if (!Histogram[i]) continue;
				maximum = i;
				break; }
		maxstretch = maximum;
		m_sliderMax->setValue  (maximum);
		m_spinBoxMax->setValue (maximum);
	}

	// error checking
  if ((minstretch < min || minstretch > max) || (maxstretch < min || maxstretch > max)) return 0;

	// checking that minimum value from slider is atleast 1 less than maximum value from slider
  if (minstretch >= maxstretch)
     {maxstretch = minstretch + 1;}

 	// minstretch and maxstretch are the minimum or maximum pixel values from either the slider or image appropriately
	// apply filter
  histogramstretching(I1, minstretch, maxstretch, I2);
	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramStretching::createGroupBox:
//
// Create group box for control panel.
//
QGroupBox*
HistogramStretching::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("HistogramStretching");

	// init widgets
	// create label[i] for Minimum
	QLabel *labelMin = new QLabel;
	labelMin->setText(QString("Min"));

	// create label[i] for Maximum
	QLabel *labelMax = new QLabel;
	labelMax->setText(QString("Max"));

	// create Min slider
	m_sliderMin = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_sliderMin->setTickPosition(QSlider::TicksBelow);
	m_sliderMin->setTickInterval(25);
	m_sliderMin->setMinimum(Min);
	m_sliderMin->setMaximum(Max);
	m_sliderMin->setValue (0);

	// create Min spinbox
	m_spinBoxMin = new QSpinBox(m_ctrlGrp);
	m_spinBoxMin->setMinimum(Min);
	m_spinBoxMin->setMaximum(Max);
	m_spinBoxMin->setValue  (0);

	// create Minautovalue checkbox
	m_checkBoxMin = new QCheckBox(m_ctrlGrp);

	// create Max slider
	m_sliderMax = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_sliderMax->setTickPosition(QSlider::TicksBelow);
	m_sliderMax->setTickInterval(25);
	m_sliderMax->setMinimum(Min);
	m_sliderMax->setMaximum(Max);
	m_sliderMax->setValue  (MaxGray);

	// create Max spinbox
	m_spinBoxMax = new QSpinBox(m_ctrlGrp);
	m_spinBoxMax->setMinimum(Min);
	m_spinBoxMax->setMaximum(Max);
	m_spinBoxMax->setValue  (MaxGray);

	// create Maxautovalue checkbox
	m_checkBoxMax = new QCheckBox(m_ctrlGrp);

	// init signal/slot connections for Min
	connect(m_sliderMin , SIGNAL(valueChanged(int)), this, SLOT(changeMin (int)));
	connect(m_spinBoxMin, SIGNAL(valueChanged(int)), this, SLOT(changeMin (int)));
	connect(m_checkBoxMin, SIGNAL(stateChanged(int)), this, SLOT(changeMin (int)));

	// init signal/slot connections for Max
	connect(m_sliderMax , SIGNAL(valueChanged(int)), this, SLOT(changeMax (int)));
	connect(m_spinBoxMax, SIGNAL(valueChanged(int)), this, SLOT(changeMax (int)));
	connect(m_checkBoxMax, SIGNAL(stateChanged(int)), this, SLOT(changeMax (int)));

	//assemble dialog
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(	labelMin		, 0, 0);
	layout->addWidget(	labelMax		, 1, 0);
	layout->addWidget(m_sliderMin , 0, 1);
	layout->addWidget(m_checkBoxMin, 0, 3);
	layout->addWidget(m_spinBoxMin, 0, 2);
	layout->addWidget(m_sliderMax , 1, 1);
	layout->addWidget(m_checkBoxMax, 1, 3);
	layout->addWidget(m_spinBoxMax, 1, 2);

	//assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramStretching::changeMin
//
// Slot to process change in Min caused by moving the slider.
//
void HistogramStretching::changeMin(int m)
{
	m_sliderMin ->blockSignals(true);
	m_sliderMin ->setValue		(m   );
	m_sliderMin ->blockSignals(false);
	m_spinBoxMin->blockSignals(true);
	m_spinBoxMin->setValue		(m   );
	m_spinBoxMin->blockSignals(false);


// apply filter to source image; save result in destination Image
applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

// display Output
g_mainWindowP->displayOut();

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramStretching::changeMax
//
// Slot to process change in Max caused by moving the slider.
//
void
HistogramStretching::changeMax(int m)
{
	m_sliderMax ->blockSignals(true);
	m_sliderMax ->setValue    (m   );
	m_sliderMax ->blockSignals(false);
	m_spinBoxMax->blockSignals(true);
	m_spinBoxMax->setValue    (m   );
	m_spinBoxMax->blockSignals(false);

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramStretching::histogramstretching:
//
// HistogramStretching I1 using the 2-level mapping shown below.  Output is in I2.
//! \brief	Mapping min-max to 0-255
//! \param[in]	I1  - Input image.
//! \param[in]	min - Minimum.
//! \param[in]	max - Maximum.
//! \param[out]	I2  - Output image.
//
void
HistogramStretching::histogramstretching(ImagePtr I1, int min, int max, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// compute lut[]
	// initialized lut of 256 entries
	// 1. subtract min from every pixel
	// 2. scale to [0, 1]
	// 3. map to [0, 255] range
	int i;
	for(i=0; i<MXGRAY; ++i)
	{Histogram[i] = CLIP((int)(MaxGray*(i- min)) / (max - min), 0, MaxGray) ;}


	// for each pixel intensity in I1, read its coresponding value from Histogram, and output it to I2
	// p1 is a pointer that points to current pixel in I1. p1++ is pointing to next pixel in I1
	// p2 is a pointer that points to current pixel in I2. p2++ is pointing to next pixel in I2
	int type;
	ChannelPtr<uchar> p1, p2, endd;
	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type);
		for(endd = p1 + total; p1<endd;) *p2++ = Histogram[*p1++];
		}
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramStretching::reset:
//
// Reset parameters.
//
void
HistogramStretching::reset() {}
