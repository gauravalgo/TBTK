/** @package TBTKcalc
 *  @file DiagonalizationSolver.h
 *  @brief Solves a Model using diagonalization
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_DIAGONALIZATION_SOLVER
#define COM_DAFER45_TBTK_DIAGONALIZATION_SOLVER

#include "Model.h"
#include <complex>

/** Solves a given model by Diagonalizing the Hamiltonian. The eigenvectors and
 *  eigenvectors can then either be directly extracted and used to calculate
 *  custom physical quantities, or the PropertyExtractor can be used to extract
 *  common properties. Scales as \f$O(n^3)\f$ with the dimension of the Hilbert
 *  space. */
class DiagonalizationSolver{
public:
	/** Constructor */
	DiagonalizationSolver();

	/** Destructor. */
	~DiagonalizationSolver();

	/** Set model to work on. */
	void setModel(Model *model);

	/** Set self-consistency callback. If set to NULL or never called, the
	 *  self-consistency loop will not be run. */
	void setSCCallback(bool (*scCallback)(DiagonalizationSolver *diagonalizationSolver));

	/** Set maximum number of iterations for the self-consistency loop. */
	void setMaxIterations(int maxIterations);

	/** Run calculations. Diagonalizes ones if no self-consistency callback
	 *  have been set, or multiple times until slef-consistencey or maximum
	 *  number of iterations has been reached. */
	void run();

	/** Get eigenvalues. */
	const double* getEigenValues();

	/* Get eigenvectors. **/
	const std::complex<double>* getEigenVectors();

	/** Get amplitude for given eigenvector \f$n\f$ and physical index
	 * \f$x\f$: \f$\Psi_{n}(x)\f$.
	 *  @param state Eigenstate number.
	 *  @param index Physical index.
	 */
	const std::complex<double> getAmplitude(int state, const Index &index);

	/** Get model. */
	Model *getModel();
private:
	/** Model to work on. */
	Model *model;

	/** pointer to array containing Hamiltonian. */
	std::complex<double> *hamiltonian;

	/** Pointer to array containing eigenvalues.*/
	double *eigenValues;

	/** Pointer to array containing eigenvectors. */
	std::complex<double> *eigenVectors;

	/** Maximum number of iterations in the self-consistency loop. */
	int maxIterations;

	/** Callback function to call each time a diagonalization has been
	 *  completed. */
	bool (*scCallback)(DiagonalizationSolver *diagonalizationSolver);

	/** Allocates space for Hamiltonian etc. */
	void init();

	/** Updates Hamiltonian. */
	void update();

	/** Diagonalizes the Hamiltonian. */
	void solve();
};

inline void DiagonalizationSolver::setModel(Model *model){
	this->model = model;
}

inline void DiagonalizationSolver::setSCCallback(bool (*scCallback)(DiagonalizationSolver *diagonalizationSolver)){
	this->scCallback = scCallback;
}

inline void DiagonalizationSolver::setMaxIterations(int maxIterations){
	this->maxIterations = maxIterations;
}

inline const double* DiagonalizationSolver::getEigenValues(){
	return eigenValues;
}

inline const std::complex<double>* DiagonalizationSolver::getEigenVectors(){
	return eigenVectors;
}

inline const std::complex<double> DiagonalizationSolver::getAmplitude(int state, const Index &index){
	return eigenVectors[model->getBasisSize()*state + model->getBasisIndex(index)];
}

inline Model* DiagonalizationSolver::getModel(){
	return model;
}

#endif

