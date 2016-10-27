// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// HistogramStretching.h - Histogram Stretching widget
//
// Written by: Khadeeja Din, 2016
// ======================================================================



#ifndef HISTOGRAMSTRETCHING_H
#define HISTOGRAMSTRETCHING_H

#include "ImageFilter.h"

class HistogramStretching : public ImageFilter {
	Q_OBJECT

public:
	HistogramStretching	(QWidget *parent = 0);  // constructor
	QGroupBox*	controlPanel	();		            // create control panel
	bool		applyFilter(ImagePtr, ImagePtr);    // apply filter to input to init output
	void		reset		();		                      // reset parameters

protected:
	void histogramstretching(ImagePtr I1, int min, int max, ImagePtr I2);

protected slots:
  void    changeMin (int);
  void    changeMax (int);

private:
	// histogramstretching controls
	QSlider		*m_sliderMin ;  // Min slider
	QSlider		*m_sliderMax ;	// Max slider
	QSpinBox	*m_spinBoxMin;	// Min spinbox
	QSpinBox	*m_spinBoxMax;	// Max spinbox
	QCheckBox	*m_checkBoxMin;	// Minautovalue checkbox
  QCheckBox	*m_checkBoxMax;	// Maxautovalue checkbox

	// labels for histogramstretching
	QLabel		*m_labelMin;    // Min label
	QLabel		*m_labelMax;    // Max label

	// widgets and groupbox
	QGroupBox	*m_ctrlGrp;	    // Groupbox for panel
};

#endif	                    // HISTOGRAMSTRETCHING_H
