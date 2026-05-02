# Smart-Transit
OOP Semester Project 

## Project Overview
SmartTransit is a complete enterprise fleet management system built in C++17 with Qt 6 GUI. It manages ABC City's entire public transport ecosystem including buses, metro trains, ride-hail cars, freight trucks, and electric scooters.

## OOP Concepts Implemented
- **Abstraction & Encapsulation**: All classes with private data
- **Inheritance**: 5-level vehicle and staff hierarchies
- **Polymorphism**: IFareCalculator, IPaymentProcessor, INotifiable, IReportGenerator
- **Abstract Classes**: Entity, Vehicle, IFareCalculator
- **Pure Virtual Methods**: 4 pure virtuals in Vehicle
- **Operator Overloading**: 35+ operators across all classes
- **Templates**: DynamicArray<T>, LinkedList<T>, MinHeap<T>, Graph<T>
- **Singleton Pattern**: AppController, AuthController
- **Strategy Pattern**: IFareCalculator with FlatFareBus, ZoneBasedMetro, SurgeRideHail
- **Exception Hierarchy**: 3-level, 28+ classes
- **Static Members**: ID counters, staff counter
- **Friend Functions**: Stream operators
- **dynamic_cast**: 4+ locations for safe downcasting
