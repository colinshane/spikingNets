#include "runSpikeNet.h"
using namespace arma;

int main(){

  wall_clock timer;

  /* True only for checking chaotic behaviour by removing one spike */
  bool spikeTest = false;
  bool rasterPlot = false;

  string basePath = "/home/neurociencia/tmpxor/eqtimes/";
  string netPath = basePath + toString("net7/");
  string savePathEq = netPath + toString("eq/");
  string savePathTrain = netPath + toString("train/");
  string savePathTest = netPath + toString("test/");
  string initPath = netPath + toString("init/");
  string dynPath = netPath + toString("dyn/");

  /* Create logfile */
  ofstream logfile((netPath + toString("logfile.log")).c_str());
  fstream timelogs((netPath + toString("timelogs.log")).c_str(), fstream::out);

  /* Create net from files */
  _Net myNet = loadSpikeNet(netPath, initPath);
  
  /* Equilibrium */
  float dt = 5e-4;
  float eqTime = 1;

  logfile << "Equilibrating... \n";
  myNet = equilibrateSpikeNet(myNet, dt, eqTime, netPath, initPath, savePathEq, logfile);

  logfile << "Start training... \n";

  /* FORCE parameters */
  int trainStep = 5;
  int saveRate = 10;

  /* Load trials */
  mat trial, inp, tgt, randTime;
  ivec train;
  train.load(basePath + "train_trials.dat", raw_ascii);
  
  double timeTaken;

  timelogs << "Iteration \t Time" << endl;

  /* Learning loop */

  for (int j=0; j < train.n_elem; j++)
  {
    timer.tic();

    logfile << "\n";
    logfile << "Train " << j << " of " << train.n_elem << "\n";
    logfile << "\n";

    trial.load(basePath + "trial" + toString(as_scalar(train.row(j))) + ".dat", raw_ascii);

    inp = trial.col(0);
    tgt = trial.col(1);
    inp = inp.t();
    tgt = tgt.t();

    myNet = runSpikeNet(myNet, inp, tgt, dt, trainStep, saveRate, j, netPath, dynPath, savePathTrain, spikeTest, rasterPlot, logfile);

    timeTaken = timer.toc();

    timelogs << j+1 << " \t " << timeTaken << endl;
  }

  timelogs.close();
  
  /* Load tests */
  mat test;
  ivec tests;
  tests.load(basePath + "test_trials.dat", raw_ascii);

  /* Test loop */
  trainStep = (int) INFINITY;

  for (int k = 0; k < tests.n_elem; k++)
  {
    logfile << "\n";
    logfile << "Test " << k << " of " << tests.n_elem << "\n";
    logfile << "\n";

    test.load(basePath + "trial" + toString(as_scalar(tests.row(k))) + ".dat", raw_ascii);
    
    inp = test.col(0);
    tgt = test.col(1);
    inp = inp.t();
    tgt = tgt.t();

    myNet = runSpikeNet(myNet, inp, tgt, dt, trainStep, saveRate, k, netPath, dynPath, savePathTest, spikeTest, rasterPlot, logfile);
  }
  
  logfile.close();

  return 0;
}
