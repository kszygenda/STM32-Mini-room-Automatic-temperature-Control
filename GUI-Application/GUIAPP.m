classdef GUIAPP < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        UIFigure      matlab.ui.Figure
        StartButton   matlab.ui.control.Button
        PauseButton   matlab.ui.control.Button
        ResumeButton  matlab.ui.control.Button
        SaveButton    matlab.ui.control.Button
        SetButton      matlab.ui.control.Button
        SimulateButton matlab.ui.control.Button
        SetTempSlider matlab.ui.control.Slider
        CurrentTempLabel matlab.ui.control.Label
        SetTempLabel  matlab.ui.control.Label
        TempTrendAxes matlab.ui.control.UIAxes
        SetTempEditField matlab.ui.control.EditField
        MyVector = [];
        t;

    end
        
    

    % Callbacks that handle component events
    methods (Access = private)

        % Code that executes after component creation
        function startupFcn(app)
            % Code to initialize temperature regulation

        end
        function sample_val = collect_sample(app)
            % Code to collect samples from temperature sensor
            % TODO - replace with your code - Implementacja funkcji zbierającej z portu szeregowego
            sample_val = randi(20);
        end
        % Button pushed function for StartButton
        function StartButtonPushed(app, event)
            % Code to start temperature regulation\
            app.MyVector=[];
            start_time=tic;
            app.t = timer('ExecutionMode', 'fixedRate', 'Period', 0.01, 'TimerFcn', @(~,~) myFunction(app,start_time));
            start(app.t);
        end

        % Funkcja cykliczna 
        function myFunction(app,start_time)
            % Code to collect and plot data
            sample_val = collect_sample(app);
            app.MyVector = [app.MyVector sample_val];
            new_stop_time=toc(start_time);
            app.TempTrendAxes.XLim = [0 new_stop_time];
            t_plot = linspace(0, new_stop_time, length(app.MyVector));
            app.TempTrendAxes.YLim = [0 max(app.MyVector)+1];
            plot(app.TempTrendAxes, t_plot, app.MyVector, 'b-');
            drawnow;
        end

        % Button pushed function for PauseButton
        function PauseButtonPushed(app, event)
            stop(app.t);
        end

        % Button pushed function for ResumeButton
        function ResumeButtonPushed(app, event)
            start(app.t);
        end

        % Button pushed function for SaveButton
        function SaveButtonPushed(app, event)
            % Code to save current settings or data
        end

        % Value changing function for SetTempSlider
        function SetTempSliderValueChanged(app, event)
            value = app.SetTempSlider.Value;
            % Code to update temperature setting
            app.SetTempEditField.Value = num2str(value); % Update edit field with slider value
        end
        
        % Value changing function for SetTempEditField
        function SetTempEditFieldValueChanged(app, event)
            value = str2double(app.SetTempEditField.Value);
            % Code to update temperature setting
            app.SetTempSlider.Value = value; % Update slider with edit field value
        end

        % Button pushed function for SetButton
        function SetButtonPushed(app, event)
            % Placeholder for Set button functionality
        end

        % Button pushed function for SimulateButton
        function SimulateButtonPushed(app, event)
            % Placeholder for Simulate button functionality
        end

    end

    % App initialization and construction
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)
            % Create UIFigure
            app.UIFigure = uifigure;
            app.UIFigure.Resize = 'off';
            app.UIFigure.Position = [100 100 640 480]; % Adjust the size as needed
            app.UIFigure.Name = 'Temperature Control System';

            % Create StartButton
            app.StartButton = uibutton(app.UIFigure, 'push');
            app.StartButton.Position = [50, 420, 100, 22];
            app.StartButton.Text = 'Start';
            app.StartButton.ButtonPushedFcn = createCallbackFcn(app, @StartButtonPushed, true);

            % Create PauseButton
            app.PauseButton = uibutton(app.UIFigure, 'push');
            app.PauseButton.Position = [160, 420, 100, 22];
            app.PauseButton.Text = 'Pause';
            app.PauseButton.ButtonPushedFcn = createCallbackFcn(app, @PauseButtonPushed, true);

            % Create ResumeButton
            app.ResumeButton = uibutton(app.UIFigure, 'push');
            app.ResumeButton.Position = [270, 420, 100, 22];
            app.ResumeButton.Text = 'Resume';
            app.ResumeButton.ButtonPushedFcn = createCallbackFcn(app, @ResumeButtonPushed, true);

            % Create SaveButton
            app.SaveButton = uibutton(app.UIFigure, 'push');
            app.SaveButton.Position = [380, 420, 100, 22];
            app.SaveButton.Text = 'Save';
            app.SaveButton.ButtonPushedFcn = createCallbackFcn(app, @SaveButtonPushed, true);

            % Create SetTempSlider
            app.SetTempSlider = uislider(app.UIFigure);
            app.SetTempSlider.Position = [50, 60, 540, 3];
            app.SetTempSlider.ValueChangedFcn = createCallbackFcn(app, @SetTempSliderValueChanged, true);

            % Create CurrentTempLabel
            app.CurrentTempLabel = uilabel(app.UIFigure);
            app.CurrentTempLabel.Position = [50, 380, 100, 22];
            app.CurrentTempLabel.Text = 'Current Temp';

            % Create SetTempLabel
            app.SetTempLabel = uilabel(app.UIFigure);
            app.SetTempLabel.Position = [50, 340, 100, 22];
            app.SetTempLabel.Text = 'Set Temp';

            % Create TempTrendAxes
            app.TempTrendAxes = uiaxes(app.UIFigure);
            app.TempTrendAxes.Position = [50, 100, 540, 220]; % Adjust the size as needed
            app.TempTrendAxes.XLabel.String = 'Time';
            app.TempTrendAxes.YLabel.String = 'Temperature';

            % Create SetTempEditField
            app.SetTempEditField = uieditfield(app.UIFigure, 'text');
            app.SetTempEditField.Position = [150, 340, 100, 22];
            app.SetTempEditField.ValueChangedFcn = createCallbackFcn(app, @SetTempEditFieldValueChanged, true);

            % Create SetButton
            app.SetButton = uibutton(app.UIFigure, 'push');
            app.SetButton.Position = [260, 340, 100, 22];
            app.SetButton.Text = 'Ustaw';
            app.SetButton.ButtonPushedFcn = createCallbackFcn(app, @SetButtonPushed, true);

            % Create SimulateButton
            app.SimulateButton = uibutton(app.UIFigure, 'push');
            app.SimulateButton.Position = [370, 340, 100, 22];
            app.SimulateButton.Text = 'Zasymuluj';
            app.SimulateButton.ButtonPushedFcn = createCallbackFcn(app, @SimulateButtonPushed, true);

        
        end
    end

    % App creation and deletion
    methods (Access = public)

        % Construct app
        function app = GUIAPP
            % Create and configure components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.UIFigure)

            % Run the startup function
            runStartupFcn(app, @startupFcn)

            % Show the figure after all components are created
            app.UIFigure.Visible = 'on';
        end
    end
end
