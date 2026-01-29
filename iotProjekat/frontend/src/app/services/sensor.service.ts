import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, interval } from 'rxjs';
import { switchMap, startWith } from 'rxjs/operators';

export interface SensorData {
  temperature: number;
  humidity: number;
  gas: number;
  gasLimit: number;
}

@Injectable({
  providedIn: 'root'
})
export class SensorService {
  private apiUrl = 'http://localhost:5215/api/sensor';

  constructor(private http: HttpClient) { }

  getSensorData(): Observable<SensorData> {
    return this.http.get<SensorData>(`${this.apiUrl}/data`);
  }

  getSensorDataPolling(intervalMs: number = 2000): Observable<SensorData> {
    return interval(intervalMs).pipe(
      startWith(0),
      switchMap(() => this.getSensorData())
    );
  }

  updateGasLimit(gasLimit: number): Observable<any> {
    return this.http.put(`${this.apiUrl}/gaslimit`, { gasLimit });
  }
}

