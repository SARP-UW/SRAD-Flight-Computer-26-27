// This file contains all the important values that the client may need to change.

#define APOGEE_PYRO_BACKUP_DELAY_MS 2000

#define MAIN_PYRO_ALTITUDE_M 500.0f 

#define MAIN_PYRO_BACKUP_DELAY_MS 2000

#define POST_FLIGHT_LOGGING_DURATION_MS 15000

#define LANDING_DETECTION_MAX_VELOCITY 3.0f // m/s
#define LANDING_DETECTION_INTERVAL_MS 5000U

// Safe State
#define SIMULATED_TIME_TO_APOGEE_MS 0000 // Make sure to consider equipment like airbrakes, 
                                         // that may produce results different from what was simulated. 

// Simulated time it takes for the rocket to go from apogee to the main pyro altitude.
#define APOGEE_TO_MAIN_PYRO_DELAY_MS 0000

// The total simulated flight duration, from launch to landing.
#define SIMULATED_FLIGHT_DURATION_MS 0000