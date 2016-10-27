// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Quantization.cpp - Quantization class
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#include "MainWindow.h"
#include "Quantization.h"
#include <cstdlib>

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::Quantization:
//
// Constructor.

Quantization::Quantization(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Quantization::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// get quantization value
	int quan = m_slider->value();

	// get dither value (true or false)
	int dither = m_checkBox->isChecked();

	// error checking
	if(quan < 0 || quan> MXGRAY) return 0;

	// apply filter
	quantization(I1, quan, dither, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Quantization::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Quantization");

	// init widgets
	// create label[i] for quantization Levels
	QLabel *label = new QLabel;
	label->setText(QString("Levels"));

	// create label[i] for dither checkbox
	QLabel *dlabel = new QLabel;
	dlabel->setText(QString("Dither"));

	// create slider
	m_slider = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_slider->setTickPosition(QSlider::TicksBelow);
	m_slider->setTickInterval(25);
	m_slider->setMinimum(1);
	m_slider->setMaximum(MXGRAY);
	m_slider->setValue  (4);

	// create spinbox
	m_spinBox = new QSpinBox(m_ctrlGrp);
	m_spinBox->setMinimum(1);
	m_spinBox->setMaximum(MXGRAY);
	m_spinBox->setValue  (4);

	//create checkbox for dither
	m_checkBox = new QCheckBox(m_ctrlGrp);


	// init signal/slot connections for Quantization
	connect(m_slider , SIGNAL(valueChanged(int)), this, SLOT(changeQuan (int)));
	connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(changeQuan (int)));

	// assemble dialog
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(  label  , 0, 0);
	layout->addWidget(m_slider , 0, 1);
	layout->addWidget(m_spinBox, 0, 2);
	layout->addWidget(m_checkBox, 1, 1);
	layout->addWidget(dlabel, 1, 0);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::changeQuan:
//
// Slot to process change in quan caused by moving the slider.
//
void
Quantization::changeQuan(int quan)
{
	m_slider ->blockSignals(true);
	m_slider ->setValue    (quan );
	m_slider ->blockSignals(false);
	m_spinBox->blockSignals(true);
	m_spinBox->setValue    (quan );
	m_spinBox->blockSignals(false);

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::quantization:
//
//Quantization I1 using the 2-level mapping shown below.  Output is in I2.
//! \brief	To quantize we add or subtract a bias value from each pixel
//! \details	Add dither to pixel values if dither = 1 else quantize without dither
//! \param[in]	I1  - Input image.
//! \param[in]	quan - Quantization.
//! \param[in]	dither - dither.
//! \param[out]	I2  - Output image.
//
void
Quantization::quantization(ImagePtr I1, int quan, int dither, ImagePtr I2) {
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// variable for quantization levels
	int levels = quan;

	// scale is the value to add or subtract from each pixel
  int scale = (MXGRAY + 1) / levels;

	// bias brings the scale down by a factor of 2
	// will add bias to each point
	double bias = scale/2.0;

	// compute lut[]
	int i, lut[MXGRAY];
  for(i=0;  i<=MXGRAY; ++i)
			lut[i] = scale * (int) (i/scale) + bias;

	int pixel = 0;
	// int noise is the dither noise to add to each pixel
	// int sign is sign of dither noise. It tells if to add or subtract noise
	// on odd row always add negative noise, on even row always add positive noise
	int noise, sign;

  int type;
	ChannelPtr<uchar> p1, p2, endd;

	// check if dither checkbox is checked or not
	// if not checked copy values from lut to I2
  if (!dither) {
		for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);
			for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
		}
	}

	// else if dither is checked, apply dither to each pixel value
  else {
		for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);
			for (int y=0; y<h; y++)
			{
				 // if row is odd, intiialize sign to 1
				 if (y % 2)
				 	sign = 1;

				 // else if row is even, intiialize sign to -1
				 else
				 	sign = -1;

				 for (int x=0; x<w; x++)
						{
							// 2^5 = 32767  // gives a number b/w 0-1
							noise = ((rand()&0x7fff) / 32767.) * bias;

							// alternating the noise addition or subtraction
							switch(sign)
							{
								// on odd row adding negative value
								case 1:
								 				// adding noise to pixel
												pixel = *p1++ + noise;
												sign = -1;
												break;
								// on even row adding positive value
								case -1:
								 				// subtracting noise form pixel
												pixel = *p1++ - noise;
												sign = 1;
												break;
							}

							// purpose of clipping is to make sure output pixel value does not goes off range
							// clipping the pixel value after applying the dither and before copying to output image
							*p2++ = lut[ CLIP(pixel, 0, MaxGray)];
							}
						}
					}
				}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Quantization::reset:
//
// Reset parameters.
//
void
Quantization::reset() {}
