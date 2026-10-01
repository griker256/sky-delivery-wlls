#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <cmath>
#include "Models.h"

using namespace std;

void clearInput()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string formatUGX(double amount)
{
    stringstream ss;
    ss << fixed << setprecision(0) << amount;
    string num = ss.str();
    int pos = num.length() - 3;

    while (pos > 0)
    {
        num.insert(pos, ",");
        pos -= 3;
    }

    return num;
}

string orderStatusToString(OrderStatus status)
{
    switch (status)
    {
        case OrderStatus::WaitingForPayment:
            return "Waiting For Payment";
        case OrderStatus::Paid:
            return "Paid";
        case OrderStatus::Preparing:
            return "Preparing";
        case OrderStatus::Assigned:
            return "Driver Assigned";
        case OrderStatus::PickedUp:
            return "Picked Up";
        case OrderStatus::OnDelivery:
            return "On Delivery";
        case OrderStatus::Delivered:
            return "Delivered";
        case OrderStatus::Completed:
            return "Completed";
        case OrderStatus::Cancelled:
            return "Cancelled";
    }
    return "Unknown";
}

string paymentStatusToString(PaymentStatus status)
{
    switch (status)
    {
        case PaymentStatus::Pending:
            return "Pending";
        case PaymentStatus::Paid:
            return "Paid";
        case PaymentStatus::Failed:
            return "Failed";
        case PaymentStatus::Refunded:
            return "Refunded";
    }
    return "Unknown";
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    const double R = 6371.0;
    const double PI = 3.14159265359;

    double dLat = (lat2 - lat1) * PI / 180.0;
    double dLon = (lon2 - lon1) * PI / 180.0;

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1 * PI / 180.0) * cos(lat2 * PI / 180.0) *
               sin(dLon / 2) * sin(dLon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

double calculateDeliveryFee(double distanceKm)
{
    return 5000 + (distanceKm * 1000);
}

string generateMapsLink(double latitude, double longitude)
{
    return "https://www.google.com/maps/?q=" +
           to_string(latitude) + "," +
           to_string(longitude);
}

#endif
