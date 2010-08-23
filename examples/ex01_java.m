 % OSCATS: Open-Source Computerized Adaptive Testing System
 % Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
 %
 % Example 1
 %
 % 400 Items: 1PL, b ~ N(0,1)
 % 1000 Examinees: theta ~ N(0,1)
 % Item selection:
 %  - pick randomly
 %  - match theta with b, exactly
 %  - match theta with b, randomize 5, 10 items
 % Test length: 30
 % Report:
 %  - theta.hat, Item exposure

 function ex01_java()
 
%% Add glib.jar and oscats.jar to class path
%javaaddpath('../bindings/java/glib.jar');
%javaaddpath('../bindings/java/oscats.jar');
%% Note that the path to libglibjni and liboscatsjni 
%% must be in Matlab's toolbox/local/librarypath.txt

import oscats.GslVector;
import oscats.Examinee;
import oscats.Test;
import oscats.AlgExposureCounter;

N_EXAMINEES = 1000;
N_ITEMS = 400;
LEN = 30;

function ret = gen_items()
    import oscats.ItemBank
    import oscats.ContModelL1p
    import oscats.Item
    % Create an item bank to store the items.
    % Setting the property "sizeHint" increases allocation efficiency.
    bank = ItemBank(N_ITEMS);
    for k = 1:N_ITEMS
      % First we create an IRT model container for our item
      model = ContModelL1p();
      % Then, set the parameters.  Here there is only one, the difficulty (b).
      model.setParamByIndex(0, oscats.Random.normal(1));
      % Create an item based on this model
      item = Item(model);
      % Add the item to the item bank
      bank.addItem(item);
      % Since Java and Matlab are garbage collected, we don't have to worry
      % about reference counting.
    end
    ret = bank;
end

  % Returns an array of new OscatsExaminee pointers
function examinees = gen_examinees()
    import oscats.Examinee
    import oscats.GslVector
    ret = javaArray('oscats.Examinee', N_EXAMINEES);
    
    % Latent IRT ability parameter.  This is a one-dimensional test.
    theta = oscats.GslVector.createGslVector(1);
    for k = 1:N_EXAMINEES
      % Sample the ability from N(0,1) distribution
      theta.set(0, oscats.Random.normal(1));
      % Create a new examinee
      ret(k) = Examinee();
      % Set the examinee's true (simulated) ability
      % Note, theta is *copied* into examinee.
      ret(k).setTrueTheta(theta);
    end
    
    examinees = ret;
end

%% main

num_tests = 4;
test_names = { 'random', 'matched', 'match.5', 'match.10' };
test = javaArray('oscats.Test', num_tests);
exposure = javaArray('oscats.AlgExposureCounter', num_tests);
    
disp('Creating examinees.');
examinees = gen_examinees();
disp('Creating items.');
bank = gen_items();

disp('Creating tests.');
for j = 1:num_tests
      % Create a new test with the given properties

      % In this case, we know what the length of the test will be (30),
      % so, we set length_hint for allocation efficiency.
      % Any good/reasonable guess will do---the size will be adjusted
      % automatically as necessary.
      test(j) = Test(test_names(j), bank, LEN);

      % Register the CAT algorithms for this test.
      % A test must have at minimum a selection algorithm, and administration
      % algorithm, and a stoping critierion.

      % Have to pass empty set of parameters to appease Matlab
      oscats.AlgSimulateTheta().register(test(j));
      oscats.AlgEstimateTheta().register(test(j));

      % All calls to oscats.Algorithm.register() return an algorithm
      % data object.  In many cases, we don't care about this object, since
      % it doesn't contain any interesting information.  But, for the
      % item exposure counter, we want to have access to the item exposure
      % rates when the test is over, so we keep the object.
      exposure(j) = oscats.AlgExposureCounter();
      exposure(j).register(test(j));

      alg = oscats.AlgFixedLength.createAlgFixedLength(LEN);
      alg.register(test(j));
end
    
% Here we register the item selection criteria for the different tests
oscats.AlgPickRand().register(test(1));
% The default for OscatsAlgClosestDiff is to pick the exact closest item
oscats.AlgClosestDiff().register(test(2));
% But, we can have the algorithm choose randomly from among the num closest
alg = oscats.AlgClosestDiff.createAlgClosestDiff(5);
alg.register(test(3));
alg = oscats.AlgClosestDiff.createAlgClosestDiff(10);
alg.register(test(4));
    
disp('Administering.');
f = fopen('ex01-examinees.dat', 'w');
fprintf(f, 'ID\ttrue');
for j = 1:num_tests
  fprintf(f, '\t%s\t%s.err', char(test_names(j)), char(test_names(j)));
end
fprintf(f, '\n');
for i = 1:N_EXAMINEES
      % An initial estimate for latent IRT ability must be provided.
      examinees(i).setThetaHat(GslVector.createGslVector(1));
      % We want errors for the estimates: initialize them.
      % 1 is the test dimension
      examinees(i).initThetaErr(1);

      fprintf(f, '%d\t%g', i, examinees(i).getTrueTheta().get(0));
      for j = 1:num_tests
        % Reset initial latent ability for this test
        examinees(i).getThetaHat().set(0, 0);
        % Do the administration!
        test(j).administer(examinees(i));
        % Output the resulting theta.hat and its *variance*
        fprintf(f, '\t%g\t%g', examinees(i).getThetaHat().get(0), ...
                examinees(i).getThetaErr().get(0, 0));
      end
      fprintf(f, '\n');
end
fclose(f);
    
f = fopen('ex01-items.dat', 'w');
fprintf(f, 'ID\tb');
for j = 1:num_tests
    fprintf(f, '\t%s', char(test_names(j)));
end
fprintf(f, '\n');
for i = 1:N_ITEMS
      % Get the item's difficulty parameter
      fprintf(f, '%d\t%g', i, bank.getItem(i-1).getContModel().getParamByIndex(0));
      % Get the exposure rate for this item in each test
      for j = 1:num_tests
        fprintf(f, '\t%g', exposure(j).getRate(bank.getItem(i-1)) );
      end
      fprintf(f, '\n');
end
fclose(f);

% No clean up necessary, since Java and Matlab are garbage collected.
disp('Done.');

 end