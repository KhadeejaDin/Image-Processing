// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Sharpen.cpp - Sharpen class
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#include "MainWindow.h"
#include "Sharpen.h"

extern MainWindow *g_mainWindowP;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// min and max variables for filter size and factor range
int s_min = 1;
int s_max = 99;
int f_max = 5;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::Sharpen:
//
// Constructor.

Sharpen::Sharpen(QWidget *parent) : ImageFilter(parent)
{
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Sharpen::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// get filter size
	int size = m_sliders->value();

	// get factor value
	double factor = m_sliderf->value();

	// error checking
	if(size < s_min || size > s_max) return 0;

	// apply filter
	sharpen(I1, size, factor, I2);
	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Sharpen::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Sharpen");

	// init widgets
	// create label[i] for size
	QLabel *labels = new QLabel;
	labels->setText(QString("Size"));

	// create label[i] for factor
  QLabel *labelf = new QLabel;
  labelf->setText(QString("Factor"));

	// create size slider
	m_sliders = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_sliders->setTickPosition(QSlider::TicksBelow);
	m_sliders->setTickInterval(25);
	m_sliders->setSingleStep(2);
	m_sliders->setMinimum(s_min);
	m_sliders->setMaximum(s_max);
	m_sliders->setValue  (s_min);

	// create size spinbox
  m_spinBoxs = new QSpinBox(m_ctrlGrp);
	m_spinBoxs->setSingleStep(2);
	m_spinBoxs->setMinimum(s_min);
	m_spinBoxs->setMaximum(s_max);
	m_spinBoxs->setValue  (s_min);

	// create factor slider
  m_sliderf = new QSlider(Qt::Horizontal, m_ctrlGrp);
  m_sliderf->setTickPosition(QSlider::TicksBelow);
  m_sliderf->setTickInterval(2);
	m_sliderf->setSingleStep(2);
  m_sliderf->setMinimum(s_min);
  m_sliderf->setMaximum(f_max);
  m_sliderf->setValue  (s_min);

	// create factor spinbox
  m_spinBoxf = new QSpinBox(m_ctrlGrp);
	m_spinBoxf->setSingleStep(2);
  m_spinBoxf->setMinimum(s_min);
  m_spinBoxf->setMaximum(f_max);
  m_spinBoxf->setValue (s_min);


	// init signal/slot connections for size
	connect(m_sliders , SIGNAL(valueChanged(int)), this, SLOT(changeSize (int)));
	connect(m_spinBoxs, SIGNAL(valueChanged(int)), this, SLOT(changeSize (int)));

	// init signal/slot connections for factor
  connect(m_sliderf , SIGNAL(valueChanged(int)), this, SLOT(changeFactor (int)));
  connect(m_spinBoxf, SIGNAL(valueChanged(int)), this, SLOT(changeFactor (int)));

	// assemble dialog
	QGridLayout *layout = new QGridLayout;
  layout->addWidget(	labels		, 0, 0);
  layout->addWidget(	labelf		, 1, 0);
  layout->addWidget(m_sliders , 0, 1);
  layout->addWidget(m_spinBoxs, 0, 2);
  layout->addWidget(m_sliderf , 1, 1);
  layout->addWidget(m_spinBoxf, 1, 2);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::changeSize:
//
// Slot to process change in size caused by moving the slider.
//
void
Sharpen::changeSize(int value)
{
  value += !(value %2);
  m_sliders ->blockSignals(true);
  m_sliders ->setValue    (value);
  m_sliders ->blockSignals(false);
  m_spinBoxs->blockSignals(true);
  m_spinBoxs->setValue    (value);
  m_spinBoxs->blockSignals(false);

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::changeFactor:
//
// Slot to process change in factor caused by moving the slider.
//
void
Sharpen::changeFactor(int value)
{
  m_sliderf ->blockSignals(true);
  m_sliderf ->setValue    (value);
  m_sliderf ->blockSignals(false);
  m_spinBoxf->blockSignals(true);
  m_spinBoxf->setValue    (value);
  m_spinBoxf->blockSignals(false);

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::sharpen:
//
//! \brief    Sharpen blurs the input image and save it to temp
//! \details	Subtracts temp from I1 and output it to I2
//! \param[in]	I1  - Input image.
//! \param[in]	sz - filter size.
//! \param[in]	fctr - sharpen factor.
//! \param[out]	I2  - Output image.
//

void
Sharpen::sharpen(ImagePtr I1, int sz, double fctr, ImagePtr I2) {
	ImagePtr temp;
	IP_copyImageHeader(I1, temp);
	IP_copyImageHeader(I1, I2);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// send I1 to sharpblur. The blurred image is stored in temp
	sharpblur (I1, sz, sz, temp);

	// src is pointer to I1, tempPointer is pointer to temp, dst is pointer to I2
	int type;
  ChannelPtr<uchar> src, tempPointer, dst, end;
  for(int ch = 0; IP_getChannel(I1, ch, src, type); ch++) {
		IP_getChannel(temp, ch, tempPointer, type);
  	IP_getChannel(I2, ch, dst, type);
  	for(end = src + total; src<end;)
      {	*dst = CLIP(*src + CLIP((*src - *tempPointer), 0, 255) * fctr, 0, 255);
			++src;
			++tempPointer;
			++dst;
		}
  }
}


void
Sharpen::sharpblur(ImagePtr I1, int xsz, int ysz, ImagePtr I2) {
	int w = I1->width();
	int h = I1->height();
	int total = w * h;

	// error checking
	// If window width is greater than image width than we divide it by 2 and subtract 1 to make it odd
	if (xsz > w) {
		IP_copyImage(I1, I2);
		return;
	}

	// If window height is greater than image height than we divide it by 2 and subtract 1 to make it odd
	if (ysz > h) {
		IP_copyImage(I1, I2);
		return;
	}

	// trivial case:
	// if Width and Height are 1 the window size is 0 and no blurring needs to be done
	// we simple copy input image to output image
	if (xsz <= 1 && ysz<= 1){
		if (I1 != I2)
			IP_copyImage(I1, I2);
		return;
	}

	IP_copyImageHeader(I1, I2);
	int type;
	ChannelPtr<uchar> src, dst;
	ChannelPtr<float> fsrc, fdst;

	for(int ch = 0; IP_getChannel(I1, ch, src, type); ch++) {
		// type is uchar pixel
		if (type == UCHAR_TYPE) {
			if (xsz > 1.) {
				dst = I2[ch];
				// process all rows first
				for (int y =0; y<h; y++) {
					// send one row at a time to IP_blur1D
					sharpIP_blur1D(src, w, 1, xsz, dst);
					src += w;
					dst += w;
				}
				src = I2[ch];
			}

		  if (ysz > 1.) {
				dst = I2[ch];
				// process all columns second
				for (int x =0; x<w; x++) {
					// send one column at a time to IP_blur1D
					sharpIP_blur1D(src, h, w, ysz, dst);
					src += 1;
					dst += 1;
				}
			}
		}

		// float type from image copying
		else {
			IP_castChannel(I1, ch, I2, ch, FLOAT_TYPE);
			if (xsz > 1.) {
				fdst = I2[ch];
				fsrc = I2[ch];
				// process all rows first
				for (int y =0; y<h; y++) {
					// send one row at a time to IP_blur1D
					sharpIP_blur1D(fsrc, w, 1, xsz, fdst);
					fsrc += w;
					fdst += w;
				}
				fsrc = I2[ch];
			}

			if (ysz > 1.) {
				fdst = I2[ch];
				// process all columns second
				for (int x =0; x<w; x++) {
					sharpIP_blur1D(fsrc, h, w, ysz, fdst);
					// send one column at a time to IP_blur1D
					fsrc += 1;
					fdst += 1;
				}
			}
		}
	}
}

//sharpIP_blur1D applies blur in 1 direction only
//len is the length of the Image
//stride is the distance from 1 element to next
//ww is the width of the filter
//src is the pointer to input Image
//dst is the pointer to output Image
template <class T>
void
Sharpen::sharpIP_blur1D(ChannelPtr<T> src, int len, int stride, double ww, ChannelPtr<T> dst) {

		// buffer size is length of image + filter width - 1
		size_t buf_size = len + ww -1;

		// padding is the extra spaces to pad around the image
		int padding = (ww - 1)/2;

		// error checking
		if (ww > len) {
			return;
		}

		// trivial  case
		if (ww <= 1) {
			if (src != dst) {
			for (int i =0; i<len; i++) {
				*dst = *src;
				dst += stride;
				src += stride;
			}
		}
		return;
		}

		// creating buffer
		uint16_t *buffer;
		buffer = (uint16_t*)malloc(sizeof(uint16_t*) * buf_size);
		if (buffer == NULL) {
			exit(0);
		}


		int i = 0;
		// filling up left padded spaces
		for (; i<padding; ++i) {
			buffer[i] = (*src);
		}

		// incrementing length of image by left padding
		len += i;

		// reading the remaining pixels in row from src to buffer
		for (; i<len; ++i) {
			buffer[i] = (*src);
			src += stride;
		}

		// decrementing src pixel back to the last pixel in the current row.
		src -= stride;

		// filling up right padded spaces
		padding += i;
		for (; i<padding; ++i) {
			buffer[i] = (*src);
			}

		 // SUM //
	 	 // initialize sum to 0.0
		 double sum = 0;
		 int j = 0;

		 // for each pixel in filter width add its value to sum
		 for (; j < ww; j++) {
			 sum += buffer[j];
		 }

		 // output sum/ww to dst(current output pixel)
		 *dst = sum/ww ;

		 // move output image pointer to next pixel
		 dst += stride;

		 // process all the pixels remaining in the row
		 // move filter along the row by removing the left most pixel in filter and adding the next pixel after the filter
		 for (; j< buf_size; j++, dst += stride) {
			 int last = j - ww;
			 sum += (buffer[j] - buffer[last]);
			 *dst = sum/ww;
		 }

		 // empty the buffer after 1 row is processed
		 free (buffer);
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::reset:
//
// Reset parameters.
//
void
Sharpen::reset() {}
