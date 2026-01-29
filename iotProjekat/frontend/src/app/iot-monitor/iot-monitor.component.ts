import { Component, OnInit, OnDestroy } from '@angular/core';
import { SensorService, SensorData } from '../services/sensor.service';
import { Subscription } from 'rxjs';

@Component({
  selector: 'app-iot-monitor',
  templateUrl: './iot-monitor.component.html',
  styleUrls: ['./iot-monitor.component.css']
})
export class IotMonitorComponent implements OnInit, OnDestroy {
  sensorData: SensorData = {
    temperature: 0,
    humidity: 0,
    gas: 0,
    gasLimit: 200
  };

  gasThreshold: number = 200;
  private subscription?: Subscription;

  constructor(private sensorService: SensorService) { }

  ngOnInit(): void {
    // Poll sensor data every 2 seconds
    this.subscription = this.sensorService.getSensorDataPolling(2000).subscribe({
      next: (data) => {
        const previousGasLimit = this.sensorData.gasLimit;
        this.sensorData = data;
        // Only update gasThreshold if user hasn't changed it
        // This prevents resetting the input field while user is typing
        if (this.gasThreshold === previousGasLimit) {
          this.gasThreshold = data.gasLimit;
        }
      },
      error: (error) => {
        console.error('Error fetching sensor data:', error);
      }
    });
  }

  ngOnDestroy(): void {
    if (this.subscription) {
      this.subscription.unsubscribe();
    }
  }

  blinkLED() {
  fetch("https://iotprojekat-19239-default-rtdb.europe-west1.firebasedatabase.app/ledCommand.json", {
    method: "PUT",
    headers: {
      "Content-Type": "application/json"
    },
    body: JSON.stringify(1)  
  });
}




  updateThreshold(): void {
    this.sensorService.updateGasLimit(this.gasThreshold).subscribe({
      next: () => {
        console.log('Gas threshold updated successfully');
        this.sensorData.gasLimit = this.gasThreshold;
      },
      error: (error) => {
        console.error('Error updating gas threshold:', error);
        alert('Greška pri ažuriranju praga gasa');
      }
    });
  }
}

