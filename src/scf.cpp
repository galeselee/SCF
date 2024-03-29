#include "scf.h"
#include "ttvc.h"
#include "utils.h"
#include <omp.h>

#include <cstring>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <mpi.h>
#include <chrono>

double cal_lambda(Tensor *A, Tensor *U) {
    vint shape = A->shape;
    int ndim = A->ndim;
    double lambda = 0;
    int scan[ndim];
    scan[ndim-1] = 1;
    for (int ii = ndim-2; ii >= 0; ii--) {
        scan[ii] = scan[ii+1] * shape[ii+1];
    }
    int scan_add[ndim];
    scan_add[0] = 0;
    for (int ii = 1; ii < ndim; ii++) {
        scan_add[ii] = scan_add[ii-1] + shape[A->ndim-ii];
    }
    if (ndim == 4) {
    #pragma omp parallel for default(shared) reduction(+:lambda)
        for (int ij = 0; ij < shape[0] * shape[1]; ij++) {
            int ii = ij / shape[1];
            int jj = ij % shape[1];
            int idx_ii = ii * scan[0];
            int idx_jj = jj * scan[1] + idx_ii;
            for (int kk = 0; kk < shape[2]; kk++) {
                int idx_kk = kk * scan[2] + idx_jj;
                for (int ll = 0; ll < shape[3]; ll++) {
                    lambda += A->data[idx_kk + ll] * U->data[scan_add[0]+ll] * U->data[scan_add[1]+kk]
                                * U->data[scan_add[2]+jj] * U->data[scan_add[3]+ii];
                }
            }
        }
    } else if (ndim == 5) {
#pragma omp parallel for default(shared) reduction(+:lambda)
        for (int ij = 0; ij < shape[0] * shape[1]; ij++) {
            int ii = ij / shape[1];
            int jj = ij % shape[1];
            int idx_ii = ii * scan[0];
            int idx_jj = jj * scan[1] + idx_ii;
            for (int kk = 0; kk < shape[2]; kk++) {
                int idx_kk = kk * scan[2] + idx_jj;
                for (int ll = 0; ll < shape[3]; ll++) {
                    int idx_ll = ll * scan[3] + idx_kk;
                    for (int uu = 0; uu < shape[4]; uu++) {
                        lambda += A->data[idx_ll + uu] * U->data[scan_add[0]+uu] * U->data[scan_add[1]+ll]
                                    * U->data[scan_add[2]+kk] * U->data[scan_add[3]+jj]
                                    * U->data[scan_add[4]+ii];
                    }
                }
            }
        }
    } else if (ndim == 6) {
#pragma omp parallel for default(shared) reduction(+:lambda)
        for (int ij = 0; ij < shape[0] * shape[1]; ij++) {
            int ii = ij / shape[1];
            int jj = ij % shape[1];
            int idx_ii = ii * scan[0];
            int idx_jj = jj * scan[1] + idx_ii;
            for (int kk = 0; kk < shape[2]; kk++) {
                int idx_kk = kk * scan[2] + idx_jj;
                for (int ll = 0; ll < shape[3]; ll++) {
                    int idx_ll = ll * scan[3] + idx_kk;
                    for (int uu = 0; uu < shape[4]; uu++) {
                        int idx_uu = uu * scan[4] + idx_ll;
                        for (int tt = 0; tt < shape[5]; tt++) { 
                            lambda += A->data[idx_uu + tt] * U->data[scan_add[0]+tt] * U->data[scan_add[1]+uu]
                                        * U->data[scan_add[2]+ll] * U->data[scan_add[3]+kk]
                                        * U->data[scan_add[4]+jj] * U->data[scan_add[5]+ii];
                        }
                    }
                }
            }
        }
    } else if (ndim == 7) {
        for (int ij = 0; ij < shape[0] * shape[1]; ij++) {
            int ii = ij / shape[1];
            int jj = ij % shape[1];
            int idx_ii = ii * scan[0];
            int idx_jj = jj * scan[1] + idx_ii;
            for (int kk = 0; kk < shape[2]; kk++) {
                int idx_kk = kk * scan[2] + idx_jj;
                for (int ll = 0; ll < shape[3]; ll++) {
                    int idx_ll = ll * scan[3] + idx_kk;
                    for (int uu = 0; uu < shape[4]; uu++) {
                        int idx_uu = uu * scan[4] + idx_ll;
                        for (int tt = 0; tt < shape[5]; tt++) { 
                            int idx_tt = tt * scan[5] + idx_uu;
                            for (int rr = 0; rr < shape[6]; rr++) {
                                int idx_rr = rr + idx_tt;
                                lambda += A->data[idx_rr]
                                            * U->data[scan_add[0]+rr]
                                            * U->data[scan_add[1]+tt] * U->data[scan_add[2]+uu]
                                            * U->data[scan_add[3]+ll] * U->data[scan_add[4]+kk]
                                            * U->data[scan_add[5]+jj] * U->data[scan_add[6]+ii];
                            }
                        }
                    }
                }
            }
        }
    } else if (ndim == 8) {
    #pragma omp parallel for default(shared) reduction(+:lambda)
        for (int ij = 0; ij < shape[0] * shape[1]; ij++) {
            int ii = ij / shape[1];
            int jj = ij % shape[1];
            int idx_ii = ii * scan[0];
            int idx_jj = jj * scan[1] + idx_ii;
            for (int kk = 0; kk < shape[2]; kk++) {
                int idx_kk = kk * scan[2] + idx_jj;
                for (int ll = 0; ll < shape[3]; ll++) {
                    int idx_ll = ll * scan[3] + idx_kk;
                    for (int uu = 0; uu < shape[4]; uu++) {
                        int idx_uu = uu * scan[4] + idx_ll;
                        for (int tt = 0; tt < shape[5]; tt++) { 
                            int idx_tt = tt * scan[5] + idx_uu;
                            for (int rr = 0; rr < shape[6]; rr++) {
                                int idx_rr = rr * scan[6] + idx_tt;
                                for (int ee = 0; ee < shape[7]; ee++) {
                                    int idx_ee = ee + idx_rr;
                                    lambda += A->data[idx_ee]
                                                * U->data[scan_add[0]+ee] * U->data[scan_add[1]+rr]
                                                * U->data[scan_add[2]+tt] * U->data[scan_add[3]+uu]
                                                * U->data[scan_add[4]+ll] * U->data[scan_add[5]+kk]
                                                * U->data[scan_add[6]+jj] * U->data[scan_add[7]+ii];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return lambda; 
}

double cal_res(Tensor *J, Tensor *X, double lambda) {
    Tensor w_inter({X->size});

#pragma omp parallel for default(shared)
    for (int ii = 0; ii < J->shape[0]; ii++) {
        int idx = ii * J->shape[1];
        for (int jj = ii+1; jj < J->shape[1]; jj++) {
            w_inter.data[ii] += J->data[idx+jj] * X->data[jj];
        }
    }
#pragma omp parallel for default(shared)
    for (int ii = 0; ii < J->shape[0]; ii++) {
        for (int jj = 0; jj < ii; jj++) {
            w_inter.data[ii] += X->data[jj] * J->data[jj*J->shape[0]+ii];
        }
    }
    double rho = 0.0;

#pragma omp parallel for default(shared) reduction(+:rho)
    for (int ii = 0; ii < X->size; ii++) {
        rho += w_inter.data[ii] * X->data[ii]; // X -> w
    }
#pragma omp parallel for default(shared)
    for (int ii = 0; ii < X->size; ii++) {
        w_inter.data[ii] -= rho * X->data[ii];
    }

    auto res = w_inter.fnorm_range(0, w_inter.size) / 
                (J->fnorm_range(0, J->size)*std::sqrt(2)+std::abs(lambda));
    return res;
}

void fill_J_with_block(Tensor *J, vint shapeA, int x, int y, double *block) {
    int n_J = J->shape[0];
    int x_begin = 0;
    int y_begin = 0;
    int n = shapeA.size();
    int n_x = shapeA[n-1-x];
    int n_y = shapeA[n-1-y];
    for (int i = 0; i < x; i++)
        x_begin += shapeA[n-1-i];
    for (int i = 0; i < y; i++)
        y_begin += shapeA[n-1-i];
    
    for (int i = 0; i < n_x; i++) {
        std::memcpy(J->data + (i+x_begin)*n_J + y_begin, block + i*n_y, sizeof(double) * n_y);
    }
    return ;
}

extern "C" {
	void dsyev_(const char*, const char*, const  int *, double* ,const int *, double *, double *, const int*, int *);
}

void svd_solve(Tensor *J, Tensor *eigvec, double &eig) {
    int n = J->shape[0];
    int lda = n;
    double w[n];
    char V='V';
    char U='L';
    int lwork = 3*n;
    double work[lwork];
    int info;

    dsyev_(&V, &U, &n, J->data, &lda, w, work, &lwork, &info);
    if (info != 0) {
        std::cout << "Error syev @" << __LINE__ << std::endl;
    }
    eig = w[n-1];
    int idx = n - 1;
    if (std::abs(w[0]) > std::abs(w[n-1])) {
        eig = w[0];
        idx = 0;
    }

    memcpy(eigvec->data, &(J->data[idx*n]), sizeof(double)*n);
    return ;
}

void norm_range(double *ptr, int len) {
    double sum = 0.0;
    for (int ii = 0; ii < len; ii++)
        sum += ptr[ii] * ptr[ii];
    double fnorm = std::sqrt(sum);
    for (int ii = 0; ii < len; ii++)
        ptr[ii] /= fnorm;
}

void refact_J(Tensor &block, Tensor &block_mpi, vint shape) {
    int ndim = shape.size();
    std::vector<int> offset;
    int offset_idx = 0;

    int offset_base = 0;

    for (int ii = 0; ii < mpi_size; ii++ ) {
        for (int jj = 0; jj < tasks_list[ii].size(); jj++) {
            offset.push_back(offset_base);
            offset_base += shape[ndim-1-tasks_list[ii][jj][0]] * shape[ndim-1-tasks_list[ii][jj][1]];
        }
    }

    for(auto &list : tasks_list) {
        for (auto &task : list) {
            auto ii = task[0];
            auto jj = task[1];
            double *ptr = block_mpi.data + offset[offset_idx++];
            fill_J_with_block(&block, shape, ii, jj, ptr);
        }
    }
}

void scf(Tensor *A, Tensor *U, double tol, uint32_t max_iter) {
    int ndim = A->ndim;
    vint shape = A->shape;
    int iter = 0;
    int n_x = 0;
    int shape_scan[ndim+1];
    shape_scan[0] = 0;
    for (int ii = 0; ii < ndim; ii++) {
        n_x += U[ii].size;
        shape_scan[ii+1] = n_x;
    }

    Tensor J({n_x, n_x});
    Tensor J_mpi({n_x, n_x});
    Tensor X({n_x});

    for (int ii = 0; ii < ndim; ii++) {
        std::memcpy(X.data + shape_scan[ii],
                    U[ii].data, U[ii].size * sizeof(double));
    }

    int *size_rank = (int *)malloc(sizeof(int)* mpi_size);
    memset(size_rank, 0, mpi_size* sizeof(int));

    for (int ii = 0; ii < mpi_size; ii++) {
        for (int jj = 0; jj < tasks_list[ii].size(); jj++) {
            size_rank[ii] += U[tasks_list[ii][jj][0]].size * U[tasks_list[ii][jj][1]].size;
            // if (mpi_rank == 0) {
            //     std::cout << "tasks_list[ii][jj][0]" << tasks_list[ii][jj][0] << " " << tasks_list[ii][jj][1] << std::endl;
            //     std::cout << "size_rank[ii] = " << size_rank[ii] << std::endl;
            // }
        }
        // std::cout << "tasks list size = " << tasks_list[ii].size() << std::endl;
    }

    double lambda = cal_lambda(A, U);

    while (iter < max_iter) {
        std::memset(J.data, 0, sizeof(double) * J.size);
        int store_offset = 0;
        for (int ii = 0; ii < tasks_list[mpi_rank].size(); ii++) {
            int block_ii = tasks_list[mpi_rank][ii][0];
            int block_jj = tasks_list[mpi_rank][ii][1];
            ttvc_except_dim_mpi(A, &X, J_mpi.data+rank_offset[mpi_rank]+store_offset, 
                            block_ii, block_jj);
            norm_range(J_mpi.data+rank_offset[mpi_rank]+store_offset,
                       U[block_ii].size * U[block_jj].size);
            store_offset += U[block_ii].size * U[block_jj].size;
        }


        int jmpi_copy_offset = 0;
        for (int ii = 0; ii < mpi_size; ii++) {
            MPI_Bcast(J_mpi.data+jmpi_copy_offset, size_rank[ii], MPI_DOUBLE, ii, MPI_COMM_WORLD);
            jmpi_copy_offset += size_rank[ii];
        }

        MPI_Barrier(MPI_COMM_WORLD);

        refact_J(J, J_mpi, shape);

		X.norm();
        auto res = cal_res(&J, &X, lambda);

        svd_solve(&J, &X, lambda);

#pragma omp parallel for default(shared)
        for (int ii = 0; ii < ndim; ii++) {
			X.nmul_range(shape_scan[ii], U[ii].size, 1/X.fnorm_range(shape_scan[ii], U[ii].size));
        }
        if (mpi_rank == 0)
        std::cout << iter << "-th scf iteration: lambda is " << lambda << ", residual is " << res << std::endl;

        iter++;
    }

#pragma omp parallel for
    for (int ii = ndim-1; ii >= 0; ii--) {
        std::memcpy(U[ii].data, X.data + shape_scan[ii],
                    shape[ndim-1-ii] * sizeof(double));
    }
    return ;
}
