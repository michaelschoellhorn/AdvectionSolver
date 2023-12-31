#include "fluxes.h"

fluxes::fluxes(int M, double delta_t, double delta_x, double sigma)
{
    this->M = M;
    this->delta_t = delta_t;
    this->delta_x = delta_x;
    this->sigma = sigma;
}

fluxes::fluxes() {}

void fluxes::upwind(vector<double> u_new)
{
    for (int i = 2; i < M - 2; i++)
    {
        u_new[i] = u[i] - sigma * (u[i] - u[i - 1]);
    }
    u = u_new;
}

void fluxes::lax_wendroff(vector<double> u_new)
{
    for (int i = 2; i < M - 2; i++)
    {
        u_new[i] = u[i] - sigma / 2.0 * (u[i + 1] - u[i - 1]) + pow(sigma, 2.0) / 2.0 * (u[i + 1] - 2.0 * u[i] + u[i - 1]);
    }
    u = u_new;
}

void fluxes::beam_warming(vector<double> u_new)
{
    for (int i = 2; i < M - 2; i++)
    {
        u_new[i] = u[i] - sigma * (u[i] - u[i - 1]) - sigma / 2.0 * ((u[i] - 2.0 * u[i - 1] + u[i - 2]) / delta_x) * (delta_x - delta_t);
    }
    u = u_new;
}

void fluxes::fromm(vector<double> u_new)
{
    for (int i = 2; i < M - 2; i++)
    {
        u_new[i] = u[i] - sigma * (u[i] - u[i - 1]) - sigma / 2.0 * ((u[i + 1] - u[i - 1]) / (2.0 * delta_x) - (u[i] - u[i - 2]) / (2.0 * delta_x)) * (delta_x - delta_t);
    }
    u = u_new;
}

void fluxes::minmod(vector<double> u_new)
{
    // implemented as slope limiter
    // calculate slope
    for (int i = 1; i < M - 2; i++)
    {
        double num1 = (u[i] - u[i - 1]) / delta_x;
        double num2 = (u[i + 1] - u[i]) / delta_x;
        if ((num1 * num2) > 0.0)
        {
            if (abs(num1) < abs(num2))
            {
                slope[i] = num1;
            }
            else
            {
                slope[i] = num2;
            }
        }
        else
        {
            slope[i] = 0;
        }
    }
    // now use slope to update q
    for (int i = 2; i < M - 2; i++)
    {
        u_new[i] = u[i] - sigma * (u[i] - u[i - 1]) - sigma / 2.0 * (slope[i] - slope[i - 1]) * (delta_x - delta_t);
    }
    u = u_new;
}

void fluxes::superbee(vector<double> u_new)
{
    // implemented as slope limiter
    // calculate slope
    for (int i = 1; i < M - 2; i++)
    {
        double num1 = (u[i + 1] - u[i]) / delta_x;
        double num2 = (u[i] - u[i - 1]) / delta_x;
        double s1 = 0.0;
        double s2 = 0.0;
        // calculate minmods s1 and s2
        if ((num1 * num2) > 0.0)
        {
            if (abs(num1) < abs(2.0 * num2))
            {
                s1 = num1;
            }
            else
            {
                s1 = 2.0 * num2;
            }
            if (abs(2.0 * num1) < abs(num2))
            {
                s2 = 2.0 * num1;
            }
            else
            {
                s2 = num2;
            }
        }
        // calculate maxmod of s1 and s2
        slope[i] = 0.0;
        if ((s1 * s2) > 0)
        {
            if (abs(s1) < abs(s2))
            {
                slope[i] = s2;
            }
            else
            {
                slope[i] = s1;
            }
        }
    }
    // now use slope to update q
    for (int i = 2; i < M - 2; i++)
    {
        u_new[i] = u[i] - sigma * (u[i] - u[i - 1]) - sigma / 2.0 * (slope[i] - slope[i - 1]) * (delta_x - delta_t);
    }
    u = u_new;
}

void fluxes::mc(vector<double> u_new)
{
    // implemented as flux limiter
    // calculate flux: index i of flux vector contains flux at i-0.5
    for (int i = 2; i < M - 1; i++)
    {
        double r = (u[i - 1] - u[i - 2]) / (u[i] - u[i - 1] + 1E-16); //+1E-16 to avoid div0
        double phi = max({0.0, min({(1 + r) / 2.0, 2.0, 2.0 * r})});
        flux[i] = u[i - 1] + 0.5 * (1 - sigma) * (u[i] - u[i - 1]) * phi;
    }
    // now use flux to update q
    for (int i = 2; i < M - 2; i++)
    {
        u_new[i] = u[i] - sigma * (flux[i + 1] - flux[i]);
    }
    u = u_new;
}

void fluxes::van_leer(vector<double> u_new)
{
    // implemented as flux limiter
    // calculate flux:
    for (int i = 2; i < M - 1; i++)
    {
        double r = (u[i - 1] - u[i - 2]) / (u[i] - u[i - 1] + 1E-16); //+1E-16 to avoid div0
        double phi = (r + abs(r)) / (1 + abs(r));
        flux[i] = u[i - 1] + 0.5 * (1 - sigma) * (u[i] - u[i - 1]) * phi;
    }
    // now use flux to update q
    for (int i = 2; i < M - 2; i++)
    {
        u_new[i] = u[i] - sigma * (flux[i + 1] - flux[i]);
    }
    u = u_new;
}