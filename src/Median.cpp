// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Median.cpp - Median class
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#include "MainWindow.h"
#include "Median.h"

extern MainWindow *g_mainWindowP;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

int s_minkernel = 1;
int s_maxkernel = 99;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::Median
//
// Constructor.

Median::Median(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Median::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// get kernel size
	int size = m_slidersz->value();

	// get average neighborhoods count
	int avg_nbrs = m_slideravg->value();

	// max neighborhood that can be averaged
	int max_avg_nbrs = ((size * size) - 1) >> 1;

	// error checking
	if (size < s_minkernel || size > s_maxkernel || avg_nbrs < 0 || avg_nbrs > max_avg_nbrs)
		return 0;

	median (I1, size, avg_nbrs, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Median::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Median");

	// init widgets
	// create label[i] for size
	QLabel *szlabel = new QLabel;
	szlabel->setText(QString("Size"));

	// create label[i] for average numbers
  QLabel *avglabel = new QLabel;
  avglabel->setText(QString("Avg_nbrs"));

	// create slider for size
	m_slidersz = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_slidersz->setTickPosition(QSlider::TicksBelow);
	m_slidersz->setTickInterval(25);
	m_slidersz ->setSingleStep(2);
  m_slidersz->setMinimum(s_minkernel);
	m_slidersz->setMaximum(s_maxkernel);
	m_slidersz->setValue  (s_minkernel);

	// create spinbox for size
	m_spinBoxsz = new QSpinBox(m_ctrlGrp);
	m_spinBoxsz ->setSingleStep(2);
	m_spinBoxsz->setMinimum(s_minkernel);
	m_spinBoxsz->setMaximum(s_maxkernel);
	m_spinBoxsz->setValue  (s_minkernel);

	// create slider for average numbers
  m_slideravg = new QSlider(Qt::Horizontal, m_ctrlGrp);
  m_slideravg->setTickPosition(QSlider::TicksBelow);
  m_slideravg->setTickInterval(25);
	m_slideravg ->setSingleStep(2);
  m_slideravg->setMinimum(s_minkernel);
  m_slideravg->setMaximum(s_maxkernel);
  m_slideravg->setValue  (s_minkernel);

  // create spinbox for average numbers
  m_spinBoxavg = new QSpinBox(m_ctrlGrp);
	m_spinBoxavg ->setSingleStep(2);
  m_spinBoxavg->setMinimum(s_minkernel);
  m_spinBoxavg->setMaximum(s_maxkernel);
  m_spinBoxavg->setValue  (s_minkernel);


	// init signal/slot connections for kernel size
	connect(m_slidersz , SIGNAL(valueChanged(int)), this, SLOT(changeSize (int)));
	connect(m_spinBoxsz, SIGNAL(valueChanged(int)), this, SLOT(changeSize (int)));

	// init signal/slot connections for avg_nbrs
  connect(m_slideravg , SIGNAL(valueChanged(int)), this, SLOT(changeAvg_nbrs (int)));
  connect(m_spinBoxavg, SIGNAL(valueChanged(int)), this, SLOT(changeAvg_nbrs (int)));

	// assemble dialog
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(	szlabel	, 0, 0);
  layout->addWidget(	avglabel, 1, 0);
  layout->addWidget(m_slidersz , 0, 1);
  layout->addWidget(m_spinBoxsz, 0, 2);
  layout->addWidget(m_slideravg , 1, 1);
  layout->addWidget(m_spinBoxavg, 1, 2);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::changeSize:
//
// Slot to process change in kernel size
//
void
Median::changeSize(int value)
{
	// if kernel size value is even add 1 to make it odd
  value += !(value %2);

	m_slidersz ->blockSignals(true);
	m_slidersz ->setValue    (value);
	m_slidersz ->blockSignals(false);
	m_spinBoxsz->blockSignals(true);
	m_spinBoxsz->setValue    (value);
	m_spinBoxsz->blockSignals(false);

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::changeAvg_nbrs:
//
// Slot to process change in avergae neighborhood numbers
//
void
Median::changeAvg_nbrs(int value)
{
	int sz = m_slidersz->value();


	m_slideravg ->blockSignals(true);
	m_slideravg ->setValue    (value);
	m_slideravg ->blockSignals(false);
	m_spinBoxavg->blockSignals(true);
	m_spinBoxavg->setValue    (value);
	m_spinBoxavg->blockSignals(false);


	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::median:
//
//! \brief
//! \details
//! \param[in]	I1  - Input image.
//! \param[in]	sz - kernel size.
//! \param[in]	avg_nbrs - average neighbors to blur with
//! \param[out]	I2  - Output image.
//
void
Median::median(ImagePtr I1, int sz, int avg_nbrs, ImagePtr I2) {

	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	int mid = ((sz*sz) / 2) + 1;

	int i, x, y, xx, yy, t, sum, ww = 0;

	IP_copyImageHeader(I1, I2);

	// p is temporary uchar type to hold uchar pixel values
	// p1 is initially first pixel in I1
	ChannelPtr<uchar> p1, p2, p;
	for(int ch = 0; IP_getChannel(I1, ch, p1, t); ch++) {
		IP_getChannel(I2, ch, p2, t);

	int Histogram[MXGRAY];

	// process each row
	for (y =0; y<h; ++y) {

		//initialize histo with 0's
		for (int k = 0; k<MXGRAY; k++)
			{Histogram[k] = 0;}

		// fill kernel
		for (yy = 0; yy<sz; yy++) {

			// p is the pixel to read
			// p is the pixel intensity we read. Increase frequency of p in Histogram by 1.
			// p = 0 on first turn
			p = p1 + (yy * w);

			// add values to histogram.
			for (xx = 0; xx<sz; ++xx) {
				Histogram[*p++]++;
			}
	 	}

		//process remaining points in that row
		for (x=0; x<w; ++x) {
			//find median
			for (i=sum=0; i<MXGRAY; ++i) {
				sum += Histogram[i];
				if (sum >= mid)
					break;
			}
			//copy median (i) into output
			*p2++ = i;

			//decrement
			p = p1 + x;
			for (yy = 0; yy < sz; yy++) {
				Histogram[*p]--;
				p += ww;
			}

			//increment
			p = p1 + x + sz;
			for (yy = 0; yy < sz; yy++) {
				Histogram[*p]++;
				p += w;
			}
		}

		p1 += ww;
		}
	 }

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Median::reset:
//
// Reset parameters.
//
void
Median::reset() {}
