/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
 #include "positeigen.h"

// https://github.com/libigl/eigen/blob/master/unsupported/Eigen/MPRealSupport
int main(int argc, char const *argv[])
{
	// TODO VERIFY MORE
	using P=Posit<int64_t,64,1,uint64_t,true>;
	using UT=typename P::UnpackedT;
    using MatrixP=Eigen::Matrix<P,Eigen::Dynamic,Eigen::Dynamic>  ;
  	using VectorP=Eigen::Matrix<P,Eigen::Dynamic,1>        ;

  	MatrixP A = Eigen::MatrixXf::Random(10,10).cast<P>();
  	MatrixP B = MatrixP::Ones(1,1);
  	MatrixP C = MatrixP::Ones(1,1);

//  	std::cout << "Resulting A is " << (B.cwiseProduct(A)).cast<float>() << std::endl;
  	//std::cout << "Resulting B+C is " << (B+C).cast<float>() << std::endl;
  	//P A = MatrixP::Reandom(10,10);
	Eigen::VectorXd af(4),bf(4);
	af<< 3.2e8, 1, -1, 8.0e7;
	bf << 4.0e7, 1, -1, -1.6e8;
	//af<< 3.2e8, 8.0e7,1,-1;
	//bf << 4.0e7,  -1.6e8,1,-1;
	VectorP ap,bp;
	ap= af.cast<P>();
	bp= bf.cast<P>();
	std::cout << "3.2e8 ==> unpacked " << UT(af(0)) << std::endl;
	std::cout << "3.2e8 ==> unpacked ==> double " << (double)P(UT(af(0))) << std::endl;
	std::cout << "af0 is: " << ap(0).v << " " << ap(0) << std::endl;
	std::cout << "af1 is: " << ap(1).v << " " << ap(1) << std::endl;
	VectorP abp = ap.cwiseProduct(bp);

	std::cout << "af " << af.transpose() << std::endl;
	//std::cout << "ap " << ap.transpose() << std::endl;
	std::cout << "ap " << ap.cast<double>().transpose() << std::endl;
	std::cout << "----"<< std::endl;
	std::cout << "bf " << bf.transpose() << std::endl;
	//std::cout << "bp " << bp.transpose() << std::endl;
	std::cout << "bp " << bp.cast<double>().transpose() << std::endl;
	
	std::cout << "----"<< std::endl;
	std::cout << "af prod bf " << af.cwiseProduct(bf).transpose() << std::endl;
	std::cout << "af dot bf: " << af.dot(bf) << std::endl;
	std::cout << "----"<< std::endl;

	std::cout << "ap prod bp (double)" << abp.cast<double>().transpose() << std::endl;
	std::cout << "one is " << std::hex << P::PT::POSIT_ONE << std::endl;
	std::cout << "ap prod bp (posit)" << posit_formatter<P>(abp[0]) << " "<< posit_formatter<P>(abp[1]) << " " << posit_formatter<P>(abp[2])  << " " << posit_formatter<P>(abp[3]) << std::endl;
	//std::cout << "ap prod bp " << abp.cast<P::UnpackedT>().transpose() << std::endl;
	std::cout << "sum(ap .* bp) " << abp.sum() << " " << abp.sum().unpack() << " " << (double)abp.sum() << std::endl;
	std::cout << "ap dot bp: " << ap.dot(bp).unpack() << " " << (double)(ap.dot(bp)) << std::endl; // SHOULD PRODUCE 2 and not ZERO

	return 0;
}