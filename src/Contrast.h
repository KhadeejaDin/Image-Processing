// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Contrast.h - Contrast widget
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#ifndef CONTRAST_H
#define CONTRAST_H

#include "ImageFilter.h"

class Contrast : public ImageFilter {
	Q_OBJECT

public:
	Contrast	(QWidget *parent = 0);		// constructor
	QGroupBox*	controlPanel	();		// create control panel
	bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
	void		reset		();		// reset parameters

protected:
	void contrast(ImagePtr I1, double brightness, double contrast, ImagePtr I2);

protected slots:
	void    changeCtr (int);
	void    changeBri (int);

private:
	// brightness/contrast controls
	QSlider		*m_sliderB ;	      // brightness slider
	QSlider		*m_sliderC ;	      // contrast slider
	QSpinBox	*m_spinBoxB;	      // brightness spin box
	QDoubleSpinBox	*m_spinBoxC;	// contrast   spin box

	// labels for brightness and contrast
	QLabel		*m_labelC;          // contrast label
	QLabel		*m_labelB;          // brightness label

	// widgets and groupbox
	QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif	// CONTRAST_H
