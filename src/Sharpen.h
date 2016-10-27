// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Sharpen.h - Sharpen widget
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#ifndef SHARPEN_H
#define SHARPEN_H

#include "ImageFilter.h"

class Sharpen : public ImageFilter {
	Q_OBJECT

public:
	Sharpen	(QWidget *parent = 0);	          	// constructor
	QGroupBox*	controlPanel	();		            // create control panel
	bool		applyFilter(ImagePtr, ImagePtr);    // apply filter to input to init output
	void		reset		();		                      // reset parameters

protected:
  void sharpblur(ImagePtr I1, int xsz, int ysz, ImagePtr I2);
	template <class T>
	void sharpIP_blur1D (ChannelPtr<T> src, int len, int stride, double ww, ChannelPtr<T> dst);
	void sharpen(ImagePtr I1, int sz, double fctr, ImagePtr I2);

protected slots:
	void    changeSize (int);
	void    changeFactor (int);

private:
	// blur and sharpen sliders
	QSlider		*m_sliders ;	// filter size slider
	QSlider		*m_sliderf ;	// sharpen factor slider
	QSpinBox	*m_spinBoxs;	// filter size spin box
	QSpinBox	*m_spinBoxf;	// sharpen factor spin box

	// widgets and groupbox
	QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif	// SHARPEN_H
