// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Quantization.h - Quantization widget
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#ifndef QUANTIZATION_H
#define QUANTIZATION_H

#include "ImageFilter.h"


class Quantization : public ImageFilter {
	Q_OBJECT

public:
	Quantization	(QWidget *parent = 0);		// constructor
	QGroupBox*	controlPanel	();		// create control panel
	bool		applyFilter(ImagePtr, ImagePtr);// apply filter to input to init output
	void		reset		();		// reset parameters

protected:
	void quantization(ImagePtr I1, int quan, int dither, ImagePtr I2);

protected slots:
	void changeQuan(int);

private:
	// Quantization controls
	QSlider		*m_slider ;	// Quantization sliders
	QSpinBox	*m_spinBox;	// Quantization spin boxes

	QCheckBox *m_checkBox; //Dither checkbox

	// label for Quantization
	QLabel		*m_label;	// Label for printing Quantization
	QLabel		*m_dlabel; // label for dither check box
	
	// widgets and groupbox
	QGroupBox	*m_ctrlGrp;	// Groupbox for panel
};


#endif	// QUANTIZATION_H
