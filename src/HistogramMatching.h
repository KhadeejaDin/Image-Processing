// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// HistogramMatching.h - HistogramMatching widget
//
// Written by: Khadeeja Din, 2016
// ======================================================================



#ifndef HISTOGRAMMATCHING_H
#define HISTOGRAMMATCHING_H

#include "ImageFilter.h"

class HistogramMatching : public ImageFilter {
	Q_OBJECT

public:
	HistogramMatching	(QWidget *parent = 0);		      // constructor
	QGroupBox*	controlPanel	();		                  // create control panel
	bool		applyFilter(ImagePtr, ImagePtr);          // apply filter to input to init output
	void		reset		();		                            // reset parameters

protected:
	void histogrammatching(ImagePtr I1, int n, ImagePtr I2);

protected slots:
	void changeN(int);

private:

	QSlider		*m_sliderN ;	                         // slider to read n
	QSpinBox	*m_spinBoxN;                           // spinbox to read n
	QLabel		*m_label;                              // label n
	QGroupBox	*m_ctrlGrp;
};

#endif	                                           // HISTOGRAMMATCHING_H
