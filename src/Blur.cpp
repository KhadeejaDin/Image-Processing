// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Blur.cpp - Blur class
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#include "MainWindow.h"
#include "Blur.h"

extern MainWindow *g_mainWindowP;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// min and max variables for filter size range
int minfilter = 1;
int maxfilter = 99;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blur::Blur
//
// Constructor.

Blur::Blur(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blur::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Blur::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// get filter width
	int xsz = m_sliderW->value();

	// get filter height
	int ysz = m_sliderH->value();

	// error checking
	if (xsz < minfilter || xsz > maxfilter || ysz < minfilter || ysz > maxfilter)
		return 0;

	// apply filter
	blur (I1, xsz, ysz, I2);
	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blur::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Blur::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Blur");

	// init widgets
	// create label[i] for width
	QLabel *wlabel = new QLabel;
	wlabel->setText(QString("Width"));

	// create label[i] for height
  QLabel *hlabel = new QLabel;
  hlabel->setText(QString("Height"));

	// create width slider
	m_sliderW = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_sliderW->setTickPosition(QSlider::TicksBelow);
	m_sliderW->setTickInterval(25);
	m_sliderW ->setSingleStep(2);
  m_sliderW->setMinimum(minfilter);
	m_sliderW->setMaximum(maxfilter);
	m_sliderW->setValue  (minfilter);

	// create width spinbox
	m_spinBoxW = new QSpinBox(m_ctrlGrp);
	m_spinBoxW->setMinimum(minfilter);
	m_spinBoxW->setMaximum(maxfilter);
	m_spinBoxW->setValue  (minfilter);
	m_spinBoxW->setSingleStep(2);

	// create height slider
  m_sliderH = new QSlider(Qt::Horizontal, m_ctrlGrp);
  m_sliderH->setTickPosition(QSlider::TicksBelow);
  m_sliderH->setTickInterval(25);
	m_sliderH ->setSingleStep(2);
  m_sliderH->setMinimum(minfilter);
  m_sliderH->setMaximum(maxfilter);
  m_sliderH->setValue  (minfilter);

  // create height spinbox
  m_spinBoxH = new QSpinBox(m_ctrlGrp);
  m_spinBoxH->setMinimum(minfilter);
  m_spinBoxH->setMaximum(maxfilter);
  m_spinBoxH->setValue  (minfilter);
	m_spinBoxH->setSingleStep(2);

	// create checkbox
	m_checkBox = new QCheckBox(m_ctrlGrp);
	m_checkBox->setChecked(true);

	// init signal/slot connections for Width
	connect(m_sliderW , SIGNAL(valueChanged(int)), this, SLOT(changeWidth (int)));
	connect(m_spinBoxW, SIGNAL(valueChanged(int)), this, SLOT(changeWidth (int)));

	// init signal/slot connections for Height
  connect(m_sliderH , SIGNAL(valueChanged(int)), this, SLOT(changeHeight (int)));
  connect(m_spinBoxH, SIGNAL(valueChanged(int)), this, SLOT(changeHeight (int)));

	// init signal/slot connections for checkbox
	connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(changeBoth (int)));

	// assemble dialog
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(	wlabel		, 0, 0);
  layout->addWidget(	hlabel		, 1, 0);
  layout->addWidget(m_sliderW , 0, 1);
  layout->addWidget(m_spinBoxW, 0, 2);
  layout->addWidget(m_sliderH , 1, 1);
  layout->addWidget(m_spinBoxH, 1, 2);
	layout->addWidget(m_checkBox, 1, 3);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blur::changeWidth:
//
// Slot to process change in width caused by moving the slider.
// only dealing with odd size filter boxes
//
void
Blur::changeWidth(int value)
{
	// if width value from slider is even add 1 to make it odd
  value += !(value %2);
	m_sliderW ->blockSignals(true);
	m_sliderW ->setValue    (value);
	m_sliderW ->blockSignals(false);
	m_spinBoxW->blockSignals(true);
	m_spinBoxW->setValue    (value);
	m_spinBoxW->blockSignals(false);

	// if checkbox is checked then set the height slider to same value as width slider
	if(m_checkBox->isChecked())
	{
		m_sliderH ->  setValue(value);
	}

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blur::changeHeight:
//
// Slot to process change in height caused by moving the slider.
// only dealing with odd size filter boxes
//
void
Blur::changeHeight(int value)
{
	// if height value from slider is even add 1 to make it odd
  value += !(value %2);
	m_sliderH ->blockSignals(true);
	m_sliderH ->setValue    (value);
	m_spinBoxH->blockSignals(true);
	m_spinBoxH->setValue    (value);
	m_spinBoxH->blockSignals(false);
	m_sliderH ->blockSignals(false);

	// if checkbox is checked then set the width slider to same value as height slider
	if (m_checkBox->isChecked())
  {
		m_sliderW ->  setValue(value);
  }

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blur::changeBoth:
//
// Slot to process change in height and width equally.
//
void
Blur::changeBoth(int value)
{
	// checks if the checkbox is checked
	if (value)
	{
		// copy the value from width slider to height slider
		m_sliderH->setValue (m_sliderW->value());
	}

	// apply filter to source image; save result in destination image
	applyFilter(g_mainWindowP->imageSrc(), g_mainWindowP->imageDst());

	// display output
	g_mainWindowP->displayOut();
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blur::blur:
//
//! \brief    Blur sends all rows first to IP_blur1D then it sends all columns to IP_blur1D
//! \details	First apply blur horizontally and then apply blur vertically and output to I2.
//! \param[in]	I1  - Input image.
//! \param[in]	xsz - filter Width.
//! \param[in]	ysz - filter Height.
//! \param[out]	I2  - Output image.
//
void
Blur::blur(ImagePtr I1, int xsz, int ysz, ImagePtr I2) {
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
					IP_blur1D(src, w, 1, xsz, dst);
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
					IP_blur1D(src, h, w, ysz, dst);
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
					IP_blur1D(fsrc, w, 1, xsz, fdst);
					fsrc += w;
					fdst += w;
				}
				fsrc = I2[ch];
			}

			if (ysz > 1.) {
				fdst = I2[ch];
				// process all columns second
				for (int x =0; x<w; x++) {
					// send one column at a time to IP_blur1D
					IP_blur1D(fsrc, h, w, ysz, fdst);
					fsrc += 1;
					fdst += 1;
				}
			}
		}
	}
}




//IP_blur1D applies blur in 1 direction only
//len is the length of the Image
//stride is the distance from 1 element to next
//ww is the width of the filter
//src is the pointer to input Image
//dst is the pointer to output Image
template <class T>
void
Blur::IP_blur1D(ChannelPtr<T> src, int len, int stride, double ww, ChannelPtr<T> dst) {

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
// Blur::reset:
//
// Reset parameters.
//
void
Blur::reset() {}
