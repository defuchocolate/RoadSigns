/*
 * GPSCoord.h
 *
 *  Author: Pedro Cabral
 */

#ifndef GPSCOORD_H_
#define GPSCOORD_H_

class GPSCoord {
	double latitude;
	double longitude;
public:
	GPSCoord();
	GPSCoord(double,double);
	GPSCoord(const GPSCoord&);
	void setLatitude(double lat);
	double getLatitude() const;
	void setLongitude(double lon);
	double getLongitude() const;
	GPSCoord& operator=(const GPSCoord&);
};

#endif /* GPSCOORD_H_ */
