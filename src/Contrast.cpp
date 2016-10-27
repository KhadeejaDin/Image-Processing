// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Contrast.cpp - Brightness/Contrast class.
//
// Written by: Khadeeja Din, 2016
// ======================================================================



#include "MainWindow.h"
#include "Contrast.h"

extern MainWindow *g_mainWindowP;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//bounds for brightness slider and spinbox
int min = -100;
int max = 100;

//bounds for contrast slider and spinbox
double cmin = 0.0;
double cmax = 5.0;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::Contrast:
//
// Constructor.
//
Contrast::Contrast(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Contrast::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if (I1.isNull()) return 0;

	// brightness and contrast parameters
	double b, c;

  //get brightness value
	b = m_sliderB->value();

	// get contrast value
	c = m_sliderC->value();

	// error checking
  if ((b < min || b > max) || (c < min || c > max)) return 0;

	// apply filter
  contrast(I1, b, c, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::createGroupBox:
//
// Create group box for control panel.
//
QGroupBox*
Contrast::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Contrast");

	// init widgets
	// create label[i] for contrast
	QLabel *labelC = new QLabel;
	labelC->setText(QString("Ctr"));

	// create label[i] brightness
	QLabel *labelB = new QLabel;
	labelB->setText(QString("Bri"));

	// create contrast slider
	m_sliderC = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_sliderC->setTickPosition(QSlider::TicksBelow);
	m_sliderC->setTickInterval(25);
	m_sliderC->setMinimum(min);
	m_sliderC->setMaximum(max);
	m_sliderC->setValue (max>>min);

	// create contrast spinbox
	m_spinBoxC = new QDoubleSpinBox(m_ctrlGrp);
	m_spinBoxC->setMinimum(cmin);              //cmin is 0.0
	m_spinBoxC->setMaximum(cmax);              //cmax is 5.0
	m_spinBoxC->setDecimals(1);
	m_spinBoxC->setValue (2.5);

	// create brightness slider
	m_sliderB = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_sliderB->setTickPosition(QSlider::TicksBelow);
	m_sliderB->setTickInterval(25);
	m_sliderB->setMinimum(min);
	m_sliderB->setMaximum(max);
	m_sliderB->setValue  (max>>min);

	// create brightness spinbox
	m_spinBoxB = new QSpinBox(m_ctrlGrp);
	m_spinBoxB->setMinimum(min);
	m_spinBoxB->setMaximum(max);
	m_spinBoxB->setValue  (max>>min);

	// init signal/slot connections for Contrast
	connect(m_sliderC , SIGNAL(valueChanged(int)), this, SLOT(changeCtr (int)));
	connect(m_spinBoxC, SIGNAL(valueChanged(double)), this, SLOT(changeCtr (int)));

	// init signal/slot connections for Brightness
	connect(m_sliderB , SIGNAL(valueChanged(int)), this, SLOT(changeBri (int)));
	connect(m_spinBoxB, SIGNAL(valueChanged(int)), this, SLOT(changeBri (int)));

	// assemble dialog
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(	labelC		, 0, 0);
	layout->addWidget(	labelB		, 1, 0);
	layout->addWidget(m_sliderC , 0, 1);
	layout->addWidget(m_spinBoxC, 0, 2);
	layout->addWidget(m_sliderB , 1, 1);
	layout->addWidget(m_spinBoxB, 1, 2);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast:changeCtr:
//
// Slot to process change in c caused by moving the slider.
//
void Contrast::changeCtr(int c)
{
	// contr variable to scale our contrast factor: c
  double contr;

	// initializing contr
	if (c >= 0)
		contr = c/25.0 + 1.0;
	else
		contr = c/133.0 + 1.0;

	m_sliderC ->blockSignals(true);
	m_sliderC ->setValue		(c   );
	m_sliderC ->blockSignals(false);
	m_spinBoxC->blockSignals(true);
	m_spinBoxC->setValue		(contr);
	m_spinBoxC->blockSignals(false);

// apply filter to source image; save result in destination Image
applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

// display Output
g_mainWindowP->displayOut();

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast:changeBri:
//
// Slot to process change in b caused by moving the slider.
//
void
Contrast::changeBri(int b)
{
	m_sliderB ->blockSignals(true);
	m_sliderB ->setValue    (b );
	m_sliderB ->blockSignals(false);
	m_spinBoxB->blockSignals(true);
	m_spinBoxB->setValue    (b );
	m_spinBoxB->blockSignals(false);

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::contrast:
//
// Contrast I1 using the 2-level mapping shown below.  Output is in I2.
//! \details	Output is in I2. val = (pixel - reference)* contr + reference + brightness
//! \param[in]	I1  - Input image.
//! \param[in]	brightness - Brightness.
//! \param[in]	contrast - Contrast.
//! \param[out]	I2  - Output image.
//
void
Contrast::contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// fixed reference pixel intensity value.
	// this is the intersection point between brightness & contrast
	int reference = 128;

	// contr variable to scale our contrast factor: contrast
	double contr;

	// initializing contr
	if (contrast >= 0)
		contr = contrast/25.0 + 1.0;
	else
		contr = contrast/133.0 + 1.0;

	// compute lut[], i.e. lookup table
	// initialized lut of 256 entries
	int i, lut[MXGRAY];

	// applying brightness & contrats algorithm to pixel intensities and storing their corresponding values in lut
	// the algorithm darkens levels below our reference point and brightens levels above our reference point
	// CLIP the value between 0 - 255 to make sure the value does not go off range
	for(i=0; i<MXGRAY; ++i)
		lut[i] = (int)CLIP((i - reference)* contr + reference + brightness, 0, 255);

	// for each pixel intensity in I1, read its coresponding value from lut, and output it to I2
	// p1 is a pointer that points to current pixel in I1. p1++ is pointing to next pixel in I1
	// p2 is a pointer that points to current pixel in I2. p2++ is pointing to next pixel in I2
	// initially p1 points to beginning of ch array
	int type;
	ChannelPtr<uchar> p1, p2, endd;
	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type);
		for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contrast::reset:
//
// Reset parameters.
//
void
Contrast::reset() {}
