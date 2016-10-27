// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Median.h - Median widget
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#ifndef MEDIAN_H
#define MEDIAN_H

#include "ImageFilter.h"

class Median : public ImageFilter {
	Q_OBJECT

public:
	Median	(QWidget *parent = 0);		             // constructor
	QGroupBox*	controlPanel	();	    	           // create control panel
	bool		applyFilter(ImagePtr, ImagePtr);       // apply filter to input to init output
	void		reset		();		                         // reset parameters

protected:
	void median(ImagePtr I1, int sz, int avg_nbrs, ImagePtr I2);

protected slots:
	void    changeSize (int);
	void    changeAvg_nbrs (int);

private:
	// median controls
	QSlider		*m_slidersz ;	                      // kernel size slider
	QSlider		*m_slideravg ;	                    // slider for no. of neighborhoods to average
	QSpinBox	*m_spinBoxsz;	                      // kernel size spinbox
	QSpinBox	*m_spinBoxavg;	                    // spin box for no. of neighborhoods to average
	QLabel		*szlabel;
	QLabel		*avglabel;

	// widgets and groupbox
	QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif	// MEDIAN_H
