// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// HistogramMatching.cpp - HistogramMatching class
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#include "MainWindow.h"
#include "HistogramMatching.h"
#include <cmath>
#include <cstdio>

extern MainWindow *g_mainWindowP;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Min and Max for n range
int HMin = -100;
int HMax = 100;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramMatching::HistogramMatching:
//
// Constructor.
//
HistogramMatching::HistogramMatching(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramMatching::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
HistogramMatching::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if (I1.isNull()) return 0;

	// get n value
	int n = m_sliderN->value();

	// error checking
	if(n < HMin || n > HMax) return 0;

	// apply filter
  histogrammatching(I1, n, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramMatching::createGroupBox:
//
// Create group box for control panel.
//
QGroupBox*
HistogramMatching::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("HistogramMatching");

	// init widgets
	// create label[i] for n
	QLabel *label = new QLabel;
	label->setText(QString("n"));


	// create slider for n
	m_sliderN = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_sliderN->setTickPosition(QSlider::TicksBelow);
	m_sliderN->setTickInterval(25);
	m_sliderN->setMinimum(HMin);
	m_sliderN->setMaximum(HMax);
	m_sliderN->setValue  (0);

	// create spinbox for n
	m_spinBoxN = new QSpinBox(m_ctrlGrp);
	m_spinBoxN->setMinimum(HMin);
	m_spinBoxN->setMaximum(HMax);
	m_spinBoxN->setValue  (0);

	// init signal/slot connections for HistogramMatching
	connect(m_sliderN , SIGNAL(valueChanged(int)), this, SLOT(changeN (int)));
	connect(m_spinBoxN, SIGNAL(valueChanged(int)), this, SLOT(changeN (int)));

	// assemble dialog
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(  label  , 0, 0);
	layout->addWidget(m_sliderN , 0, 1);
	layout->addWidget(m_spinBoxN, 0, 2);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramMatching::changeN:
//
// Slot to process change in n caused by moving the slider.
//
void
HistogramMatching::changeN(int n)
{
	m_sliderN ->blockSignals(true);
	m_sliderN ->setValue    (n );
	m_sliderN ->blockSignals(false);
	m_spinBoxN->blockSignals(true);
	m_spinBoxN->setValue    (n );
	m_spinBoxN->blockSignals(false);

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramMatching::HistogramMatching:
//
// HistogramMatching. Output is in I2.
//! \brief	Mapping image to specified histogram.
//! \param[in]	I1  - Input image.
//! \param[in]	n - n.
//! \param[out]	I2  - Output image.
//
void
HistogramMatching::histogrammatching(ImagePtr I1, int n, ImagePtr I2)
{

	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	IP_copyImageHeader(I1, I2);

	int left[MXGRAY], right[MXGRAY];
	int Hsum;
	double Havg, scale, temp;
	// output histogram
	int h1[MXGRAY];
	// target histogram
	int h2[MXGRAY];

	// clear histogram h1
	for (int i = 0; i<MXGRAY; i++)
	  {h1[i] = 0;}

	// evaluate histogram h1
	int type;
	ChannelPtr<uchar> p1, p2, endd; //p1 is a pointer that points to pixel. p1++ is pointing to next pixel
	 for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
	   for(endd = p1 + total; p1<endd; p1++)
	      { h1[*p1] ++; }
	 }

		double average =  (double)total / MXGRAY;

		// If n = 0 : histogram equalization
		if (n == 0) {
			for (int i = 0; i < MXGRAY - 1; ++i) {
				h2[i] = (int)average;
			}
			h2[MXGRAY - 1] = total - (int) average*(MXGRAY - 1);
		}

		// if n > 0 exponentially increasing histogram
		else if (n > 0) {
			for (int i=Havg = 0; i <MXGRAY; ++i) {
				temp = ROUND(pow((double) i/MXGRAY, (double) n) * MXGRAY);
				h2[i] = temp;
				Havg += h2[i];
			}
			scale = (double) total / Havg;
			if (scale != 1) {
				for (int i = 0; i < MXGRAY; ++i)
					h2[i] *= scale;
			}
		}

		// if n <0 exponentially decreasing histogram
		else if (n < 0) {
			Havg = 0;
			for (int i =0; i < MXGRAY; ++i) {
				temp = ROUND(pow(1 - (double) i/MXGRAY, (double) abs(n)) * MXGRAY);
				h2[i] =  temp;
				Havg += h2[i];
			}
			scale = (double) total / Havg;
			if (scale != 1) {
				for (int i = 0; i < MXGRAY; ++i)
					h2[i] *= scale;
			}
		}

		int R = 0;
		Hsum = 0;
		int p;

		// evaluate mapping of all input gray levels.
		// each input gray value maps to an interval of valid output values.
		// The endpoints of the intervals are left[] and right[]
		for (int i =0; i < MXGRAY; ++i) {
			// left end of interval
			left[i] = R;
			// cumulative value for interval
			Hsum += h1[i];
			// compute width of interval in R and adjust Hsum ad the interval widens
			while (Hsum > h2[R] && R< (MXGRAY - 1)) {
				Hsum -= h2[R];
				R++;
			}
			// initializa right end of interval
			right[i] = R;
		}

		// clear h1 and reuse it below
		for (int i =0; i < MXGRAY; ++i) {
			h1[i] = 0;
		}

		// visit all input pixels and output the transformed pixel to I2
		for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++)
		{
			IP_getChannel(I2, ch, p2, type);
			for(endd = p1 + total; p1<endd; p1++)
		  {
		    p = left[*p1];
		    if (h1[p] < h2[p])
		      {
		        *p2++ = p;
		      }

		    else
		      {
		        p = left[*p1] = MIN(p+1, right[*p1]);
						*p2++ = p;
		      }
		    h1[p]++;
		  }
		}
 }



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HistogramMatching::reset:
//
// Reset parameters.
//
void
HistogramMatching::reset() {}
