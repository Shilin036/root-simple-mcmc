#ifndef TDummyLogLikelihood_H_seen
#define TDummyLogLikelihood_H_seen
// A dummy log likelihood for testing.  This is an example, but don't
// slavishly copy it (or you will be sorry!).
class TDummyLogLikelihood {
public:
    // Determine the number of dimensions.  This is where the dimensions are
    // defined, and everything else uses it.
    std::size_t GetDim() const {return 50;}

    // Calculate the log(likelihood).  The dummy likelihood is a Gaussian
    // (with covariance) centered at zero.  The covariance is set in Init()
    // (below).  
    double operator()(const Vector& point)  const {
        double logLikelihood = 0.0;

        for (std::size_t i = 0; i<GetDim(); ++i) {
            for (std::size_t j = 0; j<GetDim(); ++j) {
                logLikelihood -= 0.5*point[i]*Error(j,i)*point[j];
            }
        }

        return logLikelihood;
    }

    // Note that this needs to be the grad(log(Likelihood)).  
    bool operator() (Vector& g, const Vector& p) {
        for (int i=0; i<p.size(); ++i) {
            g[i] = 0.0;
            for (int j=0; j<p.size(); ++j) {
                g[i] -= Error(i,j)*p[j];
            }
        }
        return true;
    }

    void Init() {
        Covariance.ResizeTo(GetDim(),GetDim());
        Error.ResizeTo(GetDim(),GetDim());

        // Set the sigma for each variable.
        for (std::size_t i = 0; i<GetDim(); ++i) {
            double sigma = 1.0;
            // double sigma = 1.0*i + 1.0;
            Covariance(i,i) = sigma*sigma;
        }

        for (std::size_t i = 0; i<GetDim(); ++i) {
            for (std::size_t j = i+1; j<GetDim(); ++j) {
                double sig1 = std::sqrt(Covariance(i,i));
                double sig2 = std::sqrt(Covariance(j,j));
                // Now give some correlations to the likelihood.  (Uncomment
                // the one you want to try).

                // Choose a random correlation
#ifdef RANDOM_CORRELATION
#define SET_CORRELATION
                Covariance(i,j) = gRandom->Uniform(-0.999,0.999)*sig1*sig2;
#endif
                
                // Choose a correlation based on the variables.  Neighbors are 
                // not correlated, but there is more correlation as the
                // variables are further apart.

#define VERY_CORRELATED
#ifdef VERY_CORRELATED
#define SET_CORRELATION
                if (i+j==GetDim()-1) {
                    Covariance(i,j) = 0.900*sig1*sig2*(j - i)/(GetDim()-1.0);
                }
#endif
                
                // Choose no correlation
#ifndef SET_CORRELATION
                Covariance(i,j) = 0.0;
#endif

                Covariance(j,i) = Covariance(i,j);
            }
        }

        // Make sure the covariance is positive definite.
        do {
            TVectorD eigenValues(GetDim());
            Covariance.EigenVectors(eigenValues);
            bool positiveDefinite = true;
            for (std::size_t i = 0; i<GetDim(); ++i) {
                if (eigenValues(i)<0.0) {
                    positiveDefinite = false;
                }
            }
            if (positiveDefinite) break;
            for (std::size_t i = 0; i<GetDim(); ++i) {
                for (std::size_t j = i+1; j<GetDim(); ++j) {
                    Covariance(i,j) = 0.9*Covariance(i,j);
                    Covariance(j,i) = Covariance(i,j);
                }
            }
        } while (true);

        Error = Covariance;
        Error.Invert();
    }
    
    static TMatrixD Covariance;
    static TMatrixD Error;
};
TMatrixD TDummyLogLikelihood::Covariance;
TMatrixD TDummyLogLikelihood::Error;
#endif
