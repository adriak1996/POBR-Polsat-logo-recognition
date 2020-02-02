#include "InvariantsCounter.h"

const double pi = 3.14159;

InvariantsCounter::InvariantsCounter(){}

Invariants InvariantsCounter::GetInvariants(std::string fileName, uint objectId)
{
	cv::Mat image = cv::imread(fileName);
	return InvariantsCounter::GetInvariants(image, objectId);
}

Invariants InvariantsCounter::GetInvariants(cv::Mat& image, uint objectId)
{
	int L = 0, S = 0;
	double malinowskaFactor = 0.0;
	int minX = 9999999, minY = 9999999, maxX = -1, maxY = -1;

	cv::Mat_<cv::Vec3b> _image = image;

	for (int i = 0; i < _image.rows; i++)
		for (int j = 0; j < _image.cols; j++) {

			if (((_image(i, j)[0] != 0 )  && _image(i, j)[1] == objectId) || (objectId > 255 && _image(i, j)[0] == 254 && ((int)(_image(i, j)[1]) + 256) == objectId)) {

				//count non-white pixel
				++S;
				if (i > maxY) maxY = i;
				if (i < minY) minY = i;
				if (j > maxX) maxX = j;
				if (j < minX) minX = j;

				//find and count edge pixel

				if (!InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(_image, i - 1, j, objectId)) {
					++L;
					continue;
				}
				if (!InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(_image, i - 1, j - 1, objectId)) {
					++L;
					continue;
				}
				if (!InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(_image, i, j - 1, objectId)) {
					++L;
					continue;
				}
				if (!InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(_image, i + 1, j, objectId)) {
					++L;
					continue;
				}
				if (!InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(_image, i + 1, j + 1, objectId)) {
					++L;
					continue;
				}
				if (!InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(_image, i, j + 1, objectId)) {
					++L;
					continue;
				}

				if (!InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(_image, i - 1, j + 1, objectId)) {
					++L;
					continue;
				}

				if (!InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(_image, i + 1, j - 1, objectId)) {
					++L;
					continue;
				}

			}
		}

	Invariants result;

	if (S > 49) { // there is no point in taking into account small objects
		malinowskaFactor = L / (2 * std::sqrt(S * pi)) - 1.0;

		image = _image;

		double m20 = InvariantsCounter::CountmNM(_image, 2, 0, objectId);
		double m02 = InvariantsCounter::CountmNM(_image, 0, 2, objectId);
		double m10 = InvariantsCounter::CountmNM(_image, 1, 0, objectId);
		double m01 = InvariantsCounter::CountmNM(_image, 0, 1, objectId);
		double m00 = InvariantsCounter::CountmNM(_image, 0, 0, objectId);
		double m11 = InvariantsCounter::CountmNM(_image, 1, 1, objectId);

		double m21 = InvariantsCounter::CountmNM(_image, 2, 1, objectId);
		double m12 = InvariantsCounter::CountmNM(_image, 1, 2, objectId);
		double m30 = InvariantsCounter::CountmNM(_image, 3, 0, objectId);
		double m03 = InvariantsCounter::CountmNM(_image, 0, 3, objectId);

		double i = m10 / m00;
		double j = m01 / m00;

		double M20 = m20 - (m10) * (m10) / m00;
		double M02 = m02 - (m01) * (m01) / m00;
		double M11 = m11 - m10 * m01 / m00;

		double M21 = m21 - (2.0 * m11 * i) - (m20 * j) + (2.0 * m01 * std::pow(i, 2.0));
		double M12 = m12 - (2.0 * m11 * j) - (m02 * i) + (2.0 * m10 * std::pow(j, 2.0));
		double M30 = m30 - (3 * m20 * i) + (2 * m10 * std::pow(i, 2.0));
		double M03 = m03 - (3 * m02 * j) + (2 * m01 * std::pow(j, 2.0));

		double M1 = (M20 + M02) / (m00 * m00);
		double M2 = (std::pow((M20 + M02), 2.0) + 4 * std::pow(M11, 2)) / (std::pow(m00, 4.0));
		double M3 = (std::pow((M30 + 3 * M12), 2.0) + std::pow((3 * M21 - M03), 2.0)) / (std::pow(m00, 5.0));
		double M4 = (std::pow((M30 + M12), 2.0) + std::pow((M21 - M03), 2.0)) / (std::pow(m00, 5.0));
		double M7 = (M20 * M02 - M11 * M11) / (m00 * m00 * m00 * m00);

		result.M1 = M1;
		result.M2 = M2;
		result.M7 = M7;
		
		std::cout << "############################################## " << std::endl;
		std::cout << "############################################## " << std::endl;
		std::cout << "Nazwa pliku " << '   ' << std::endl;
		std::cout << "Pole wynosi " << S << std::endl;
		std::cout << "Obwod wynosi " << L << std::endl;
		std::cout << "Wspolczynnik Malinowskiej  wynosi " << malinowskaFactor << std::endl;
		std::cout << "Pole inaczej  wynosi " << m00 << std::endl;
		std::cout << "Niezmiennik M1  wynosi " << M1 << std::endl;
		std::cout << "Niezmiennik M2  wynosi " << M2 << std::endl;
		std::cout << "Niezmiennik M3  wynosi " << M3 << std::endl;
		std::cout << "Niezmiennik M4  wynosi " << M4 << std::endl;
		std::cout << "Niezmiennik M7  wynosi " << M7 << std::endl;

	}
	else {
		result.M1 = -1;
		result.M2 = -1;
		result.M7 = -1;
	}

	result.S = S;
	result.maxX = maxX;
	result.maxY = maxY;
	result.minX = minX;
	result.minY = minY;
	result.malinowskaFactor = malinowskaFactor;
	return result;
}

bool InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(cv::Mat_<cv::Vec3b>& _Image, int i, int j, uint objectId) {


	if (i >= 0 && j >= 0 && i < (_Image.rows) && j < (_Image.cols) && ((_Image(i, j)[0] != 0 && _Image(i, j)[1] == objectId) || (_Image(i, j)[0] == 254 && ((int)(_Image(i, j)[1])+256) == objectId))) {
		return true;
	}
	else 
		return false;
}

double InvariantsCounter::CountmNM(cv::Mat_<cv::Vec3b>& Image, int p, int q, uint objectId) {

	double result = 0.0;
	bool pixelBelongsToObject = false;
	cv::Mat_<cv::Vec3b> _Image = Image;
	for (int i = 0; i < _Image.rows; ++i)
		for (int j = 0; j < _Image.cols; ++j) {

				pixelBelongsToObject = InvariantsCounter::CheckIfPixelIsNonBlackAndBelongsToObject(Image, i, j, objectId) ? 1.0 : 0.0;

			result += (pow((double)i, (double)p) * pow((double)j, (double)q) * pixelBelongsToObject);
		}

	return result;
}




