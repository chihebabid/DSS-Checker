#!/usr/bin/env python
# coding: utf-8

# In[1]:


ZERO = 10e-5
TIMEOUT = 10 * 60 # 10 minutes = 600 seconds


# In[2]:


import os
import pandas as pd
import numpy as np
import plotly.io as pio
import plotly.express as px
import plotly.graph_objs as go
from itertools import combinations 
from plotly.subplots import make_subplots

# render figures in notebook
pio.renderers.default = "notebook_connected"

# templates figures
px.defaults.template = "simple_white"
pio.templates.default = "simple_white"

# layout for all figures
LAYOUT_FIGURES = dict(
    autosize=False,
    width = 500,
    height = 500,
    xaxis = dict(
      constrain="domain",
      mirror=True,
      showexponent="all",
      exponentformat="power"
    ),
    yaxis = dict(
      scaleanchor = "x",
      scaleratio = 1,
      mirror=True,
      showexponent="all",
      exponentformat="power"
    ),
    title = dict(
      y = 0.9,
      x = 0.5,
      xanchor = 'center',
      yanchor = 'top'
    )
)


# # Auxiliary Functions

# In[3]:


def create_folder(path):
    """Creates a folder if it does not exist
    
    Parameters
    ----------
    path : str
        Path of the new folder
    
    Examples
    --------
    
    >>> create_folder('./results')
    """
    if not os.path.exists(path):
        os.makedirs(path)


# In[4]:


def create_figure(df, model):
    """Creates a scatter figure showing the time taken by each tool to verify each property of a model
    
    Parameters
    ----------
    df : pandas.Dataframe
        Dataframe containing the results of the experiments
    model : string
        model to be plotted
    
    Returns
    -------
    plotly.graph_objects.Figure
        Scatter figure
    
    Examples
    --------
    
    >>> import os
    >>> import pandas as pd
    >>> csv_file = os.path.join("results", "output.csv")
    >>> df = pd.read_csv(csv_file)
    >>> fig = create_figure(df, 'philo10')
    """
    model_df = df[df.model == model]

    figure = px.scatter(model_df, 
                        x="formula", y="time",
                        title=model, 
                        color="tool", 
                        symbol_sequence=['x'])

    figure.update_layout(yaxis_title="time (s)", title=LAYOUT_FIGURES['title'])
    return figure


# In[5]:


def get_axis_title(experiment, show_strategy=True):
    """Get the axis title of a figure depending on the experiment being plotted
    
    Parameters
    ----------
    experiment : str
        String with the experiment information
    show_strategy : bool, optional
        Flag to show the information related to the strategy used by the tool
    
    Returns
    -------
    str
        axis title
        
    Examples
    --------
    
    >>> get_axis_title('pmc-sog_otfL_couv99-default_1_1', True)
    pmc-sog (Lace, strategy: couv99-default, # cores: 1)
    """
    information = experiment.split('_')
    tool_name = information[0]
    
    info = []
    library_dic = {
        'otfL': 'Lace',
        'otfP': 'Pthreads',
        'otfC': 'Cthreads',
        'otf': 'Hybrid'
    }
    
    if (len(information) == 5):
        info.append(library_dic[information[1]])

    if (show_strategy):
        info.append('strategy: {}'.format(information[-3]))

    nb_nodes = int(information[-2])
    if (nb_nodes > 1):
        info.append('# nodes: {}'.format(nb_nodes))

    info.append('# cores: {}'.format(information[-1]))

    title = '{} ({})'.format(tool_name, ', '.join(info))
    
    return title


# In[6]:


def create_log_figure(table, table_errors, model, tool_x, tool_y, show_strategy=True, callback=None):
    """Creates a Scatter figure in logarithmic scale comparing the performance of two tools
    
    Parameters
    ----------
    table : pandas.Dataframe
        Dataframe with the times of each experiment
    table_errors : pandas.Dataframe
        Dataframe with the errors of each experiment
    model : string
        Model to be analyzed
    tool_x : string
        Tool to be compared and plotted on the x-axis
    tool_y : string
        Tool to be compared and plotted on the y-axis
    show_strategy : bool
        Flag to show the stretagy used by the tools
    timeout: int
        Timeou of the experiments
    callback : function
        Function to be called when clicking on a point
        
    Returns
    -------
    plotly.graph_objects.Figure
        Scatter figure
        
    Examples
    --------
    >>> import os
    >>> import pandas as pd
    >>> csv_file = os.path.join("results", "output.csv")
    >>> df = pd.read_csv(csv_file)
    >>> table = df.set_index(['model', 'formula', 'tool'], drop=True).unstack('tool')
    >>> fig = create_log_figure(table['time'], table['error'], 'philo10', 'pmc-sog_otfL_couv99-default_1_8', 'pmc-sog_otfP_couv99-default_1_8')
    """
    try:
        min_value = ZERO
        max_value = TIMEOUT
        
        min_value_log = np.log10(min_value)
        max_value_log = np.log10(max_value)

        table_model = table.loc[model]
        table_errors_model = table_error.loc[model]
        
        full_table_x = pd.concat([table_model[tool_x],table_model['property'], table_errors_model[tool_x]], axis=1)
        full_table_x.columns = ['time', 'property', 'error']

        full_table_y = pd.concat([table_model[tool_y],table_model['property'], table_errors_model[tool_y]], axis=1)
        full_table_y.columns = ['time', 'property', 'error']

        traces = [
            {"property": 'T', "color":"green"},
            {"property": 'F', "color":"red"},
            {"property": 'U', "color":"black"}
        ]

        figures = []
        for t in traces:
            # filter by verification output
            table_x = full_table_x[full_table_x.property == t['property']]
            table_y = full_table_y[full_table_y.property == t['property']]

            # custom data
            custom_data = list(zip(table_x.index, table_x.error,table_y.error))
            
            # tools
            metainfo = {
                'model': model, 
                'tools': {'x': tool_x, 'y': tool_y},
                'folder': os.path.join(os.path.abspath(os.pardir), "results")
            }

            figures.append(go.Scatter(x=table_x.time,
                                      y=table_y.time,
                                      name=t['property'],
                                      mode='markers',
                                      marker_symbol='circle-open',
                                      marker_color=t['color'],
                                      meta = metainfo,
                                      customdata=custom_data,
                                      hovertemplate =
                                        '<b>Formula # %{customdata[0]}</b><br>' +
                                        '<br><b>Times:</b><br>' +
                                        '<b>x:</b> %{x} s' +
                                        '<br><b>y:</b> %{y} s<br>' +
                                        '<br><b>Errors:</b><br>' +
                                        '<b>x:</b> %{customdata[1]}<br>' +
                                        '<b>y:</b> %{customdata[2]}',
                                        ))    

        # Line
        figures.append(go.Scatter(x=[min_value, max_value], 
                                    y=[min_value, max_value],
                                    mode='lines', showlegend=False,
                                    line=dict(color='black', width=1)))

        # Create figure
        figure = go.FigureWidget(figures)
        figure.update_layout(LAYOUT_FIGURES,
                             title_text=model,
                             hoverlabel=dict(bgcolor="white", align='auto'),
                             legend_title_text='property',
                             xaxis=dict(type='log', autorange=False, range=[min_value_log, max_value_log]),
                             yaxis=dict(type='log', autorange=False, range=[min_value_log, max_value_log]),
                             xaxis_title=get_axis_title(tool_x, show_strategy),
                             yaxis_title=get_axis_title(tool_y, show_strategy))

        # Add event
        if callback is not None:
            for i in range(len(figure.data)):
                figure.data[i].on_click(callback)
    
        return figure
    except Exception as e:
        print("Error when ploting model: {} - tool_x: {} - tool_y: {}".format(model, tool_x, tool_y))
        print(e)


# In[7]:


import webbrowser

def get_filename(base_path, tool, model, model_instance, formula):
    """Returns the absolute path of the experiment log
    
    Parameters
    ----------
    base_path : string
        Path of the folder where logs are saved
    tool : string
        Tool name
    model : string
        Model name
    model_instance : string
        Name of the model instance
    formula : string
        Identifier of the formula
    
    Returns
    -------
    string
        Absolute path of the log file
    
    """
    information = tool.split('_')
    
    tool_name = information[0]
    tool_configuration = '_'.join(information[:-2])
    nb_nodes = information[-2]
    nb_cores = information[-1]
    
    experiment_folder = os.path.join(base_path, tool_name, tool_configuration, model, model_instance)
    filename = f'{tool_name}_{model_instance}-n{nb_nodes}-th{nb_cores}-f{formula}'
    absolute_path = os.path.join(experiment_folder, filename)
    
    return absolute_path

def open_logs_callback(trace, points, selector):
    """Callback that open the log files when clicking on a point of the figure
    
    Parameters
    ----------
    trace : plotly.graph_objects.Figure
        the figure to attach the callback
    points : plotly.callbacks.Points 
        points of the figure selected
    selector: plotly.callbacks.InputDeviceState 
        Device information 
    """
    inds = points.point_inds
    if (inds):
        index = inds[0]

        formula, error_x, error_y = trace['customdata'][index]
        model_instance = trace['meta']['model']
        model = ''.join(c for c in model_instance if not c.isdigit())
        tools = trace['meta']['tools']
        logs_folder = trace['meta']['folder']

        filename_x = get_filename(logs_folder, tools['x'], model, model_instance, formula)
        filename_y = get_filename(logs_folder, tools['y'], model, model_instance, formula)

        for f in [filename_x, filename_y]:
            webbrowser.open(f'file://{f}.err')
            webbrowser.open(f'file://{f}.out')

OPEN_LOGS_CALLBACK_JS = """
function get_filename (base_path, tool, model_instance, formula) {
  const information = tool.split('_');
  const size = information.length;

  const tool_name = information[0];
  const tool_configuration = information.slice(0, size - 2).join('_');
  const nb_nodes = information[size - 2];
  const nb_cores = information[size - 1];

  const model = model_instance.replace(/[0-9]/g, '');

  const experiment_folder = `${base_path}/${tool_name}/${tool_configuration}/${model}/${model_instance}`;
  const filename = `${tool_name}_${model_instance}-n${nb_nodes}-th${nb_cores}-f${formula}`;

  return `${experiment_folder}/${filename}`;
}

const plots = document.getElementsByClassName("plotly-graph-div js-plotly-plot");
const myPlot = plots[0];

myPlot.on('plotly_click', function(data){
    const points = data.points;
    if (points.length != 1) {return ;}
    
    const myPoint = points[0];
    const formula = myPoint.customdata[0];
    const meta = myPoint.data.meta;
    
    const href = window.location.href.split('/');
    const base_path = href.splice(0,href.length-4).join('/');
    
    const filename_x = get_filename(base_path, meta.tools.x, meta.model, formula);
    const filename_y = get_filename(base_path, meta.tools.y, meta.model, formula);
    
    console.log('x: ' + filename_x);
    window.open(`${filename_x}.err`);
    window.open(`${filename_x}.out`);
    
    console.log('y: ' + filename_y);
    window.open(`${filename_y}.err`);
    window.open(`${filename_y}.out`);
});
"""


# In[8]:


def create_figure_explored_states(table_explored_states, model):
    """Creates figure showing the number of explorated states during the verification 
    
    Parameters
    ----------
    table_explored_states : pandas.Dataframe
        Dataframe with the explorated states of each experiment
    model : string
        Model to be analyzed
        
    Returns
    -------
    plotly.graph_objects.Figure
        Scatter figure
    """
    colors={'T': 'green', 'F': 'red'}
    float_formatter = "{:.2E}".format

    table_model = table_explored_states[table_explored_states.property != 'U']
    table_model = table_model[table_model.model == model]

    table_stats = table_model.groupby(['property']).agg(['mean']) 

    fig = go.Figure()
    max_x = 0
    for p in table_stats.index:
        data = table_model[table_model.property==p]
        stats = table_stats.loc[p]
        
        x_axis = np.arange(1, data['formula'].count()+1, 1)
        max_x = max(max_x, x_axis[-1]+1)
        mean = stats['explored_states','mean']
        
        figure = px.scatter(data, 
                            x=x_axis, 
                            y="explored_states",
                            title=model, 
                            color='property',
                            color_discrete_map=colors,
                            symbol_sequence=["circle"])

        line = go.Scatter(x=[x_axis[0], x_axis[-1]], 
                          y=[mean, mean],
                          mode='lines', showlegend=False,                          
                          line=dict(color=colors[p], width=1.5))

        fig.add_trace(figure['data'][0])
        fig.add_trace(line)
        
        fig.add_annotation(x=1, 
                           y=mean,
                           font=dict(color=colors[p]),
                           text=f"mean = {float_formatter(mean)}")

    fig.update_layout(title_text=model, title=LAYOUT_FIGURES['title'], 
                      width = 500, height = 500, margin=dict(r=110))
    
    fig.update_xaxes(title="formula", range=[0, max_x])
    fig.update_yaxes(title="# explored states")
    
    fig.update_annotations(dict(
        showarrow=False,
        xanchor="left",
        yanchor="middle",
        xref='paper'))
    
    return fig


# In[9]:


# Experiment filters

def versus_dfs(experiments):
    """Selects only experiments using DFS strategy"""
    exp1, exp2 = experiments
    strategy_exp1= exp1.split('_')[1]
    strategy_exp2= exp2.split('_')[1]
    
    return strategy_exp1 == 'dfs' or strategy_exp2 == 'dfs'

def versus_sequential(experiments):
    """Selects only experiments run sequentially """
    exp1, exp2 = experiments
    nodes_exp1, threads_exp1 = exp1.split('_')[-2:]
    nodes_exp2, threads_exp2 = exp2.split('_')[-2:]

    return (nodes_exp1 == '1' and nodes_exp2 == '1') and             (threads_exp1 == '1' or threads_exp2 == '1')

def same_tool(experiments, tool):
    """Selects only experiments comparing the same tool"""
    exp1, exp2 = experiments
    tool_exp1= exp1.split('_')[0]
    tool_exp2= exp2.split('_')[0]
    return tool_exp1.startswith(tool) and tool_exp2.startswith(tool)

def same_number_threads(experiments):
    """Selects only experiments comparing the same number of processes and cores"""
    exp1, exp2 = experiments
    nodes_exp1, threads_exp1 = exp1.split('_')[-2:]
    nodes_exp2, threads_exp2 = exp2.split('_')[-2:]
    return (nodes_exp1 == nodes_exp2) and (threads_exp1 == threads_exp2) 

def same_thread_library(experiments):
    """Selects only experiments comparing the same parallelization library"""
    exp1, exp2 = experiments
    library_exp1 = exp1.split('_')[1]
    library_exp2 = exp2.split('_')[1]
    return library_exp1 == library_exp2

def same_strategy(experiments):
    """Selects only experiments comparing the same strategy"""
    exp1, exp2 = experiments
    strategy_exp1 = exp1.split('_')[2]
    strategy_exp2 = exp2.split('_')[2]
    return strategy_exp1 == strategy_exp2

def only_couvreur_strategy(experiments):
    """Selects only experiments comparing couvreur emptiness check algorithm"""
    exp1, exp2 = experiments
    strategy_exp1 = exp1.split('_')[2]
    strategy_exp2 = exp2.split('_')[2]
    return strategy_exp1.startswith('couv99') and strategy_exp2.startswith('couv99')

def compare_threads_library(experiments):
    """Compares parallization libraries used in pmc-sog. 
    
    It selects experiments where the tool is only pmc-sog and the strategy, number of threads, 
    number of processus are the same.
    """
    return same_tool(experiments, 'pmc-sog') and             same_strategy(experiments) and             same_number_threads(experiments) and             not same_thread_library(experiments)

def compare_couvreur_strategies(experiments):
    """Compares couvreurs strategies used in pmc-sog. 
    
    It selects experiments where the tool is only pmc-sog, the strategy is couvreur, and 
    the parallelization library, number of threads, number of processus are the same.
    """
    return only_couvreur_strategy(experiments) and             same_thread_library(experiments) and             same_number_threads(experiments)

def compare_tools(experiments):
    """Compares pmc-sog and pnml2lts-mc using the DFS algorithm. 
    
    It selects experiments where the tools are not the same, the exploration algorithm is DFS and 
    the number of processus and cores are the same.
    """
    return not (same_tool(experiments, 'pmc-sog') or same_tool(experiments,'pnml2lts-mc')) and             versus_dfs(experiments)

def compare_multithreading(experiments):
    """Compares the sequential and multi-core version of pmc-sog. 
    
    It selects experiments where the tools is pmc-sog, the parallelization library, the emptiness check 
    strategy are the same. Here the number of processus and cores are different.
    """
    return same_tool(experiments, 'pmc-sog') and             same_thread_library(experiments) and             same_strategy(experiments) and             versus_sequential(experiments)

def compare_others(experiments):
    return (not compare_threads_library(experiments)) and         (not compare_couvreur_strategies(experiments)) and         (not compare_tools(experiments)) and         (not compare_multithreading(experiments)) and         (not compare_distributed(experiments))

# Plots to be created
plots = {
    'compare_thread_library': compare_threads_library,
    'compare_couvreur_algorithm': compare_couvreur_strategies,
    'compare_tools': compare_tools,
    'compare_multicore': compare_multithreading
}


# # Load Data

# In[10]:


# Root folder
PROJECT_FOLDER = os.path.abspath(os.pardir)

# csv file with the output
csv_file = os.path.join(PROJECT_FOLDER, "results", "output.csv")

# Output folder
OUTPUT_FOLDER = os.path.join(PROJECT_FOLDER,"results", "figures")
create_folder(OUTPUT_FOLDER)


# In[11]:


# read data
df = pd.read_csv(csv_file)

# merge the information related to the experiment (# nodes, # threads, strategy) to the tool column
df['tool'] = df[['tool', 'strategy', 'num_nodes', 'num_threads']].astype(str).apply('_'.join, axis=1)
df = df.drop(columns=['strategy', 'num_nodes', 'num_threads'])

df.head()


# In[12]:


# ground truth for properties
p_df = pd.read_csv(csv_file)
p_df =p_df[
    (p_df.tool=='pnml2lts-mc') & 
    (p_df.strategy == 'ndfs') & 
    (p_df.num_nodes == 1) & 
    (p_df.num_threads == 16)]

# only property column is needed
p_df = p_df.drop(columns=['tool', 'strategy', 'num_nodes', 'num_threads', 'time', 'explored_states', 'error'])
p_df.fillna('U', inplace=True)
p_df.set_index(['model', 'formula'], inplace=True)
p_df.sort_index(inplace=True)

p_df.head()


# In[13]:


# table with times, verification output and error for each experiment
table = df.set_index(['model', 'formula', 'tool'], drop=True).unstack('tool')
table.head()


# # Preprocessing of data

# In[14]:


# table with times for each experiment
table_time = table['time'].copy()

# replace non finished experiments with a dummy value, e.g. timeout
table_time.fillna(TIMEOUT, inplace=True)

# replace 0.00 time for 10^(-5), we cannot plot log(0)
table_time.replace(0.0, ZERO, inplace=True)

# add verification output to the table
table_time = pd.concat([table_time, p_df], axis=1)

table_time.head()


# In[15]:


# table with verification output for each experiment
table_property = table['property'].copy()

# replace non finished experiments with a dummy value
table_property.fillna('U', inplace=True)

# add ground truth to the table
table_property = pd.concat([table_property, p_df], axis=1)

table_property.head()


# In[16]:


# table with error for each experiment
table_error = table['error'].copy()

table_error.head()


# In[17]:


# table with explored states for each experiment using ltsmin
table_explored_states = table.copy()
table_explored_states = table_explored_states['explored_states']
table_explored_states = table_explored_states[['pnml2lts-mc_dfs_1_16']]
table_explored_states = table_explored_states.rename(columns={"pnml2lts-mc_dfs_1_16": "explored_states"})

# add verification output to the table
table_explored_states = pd.concat([table_explored_states, p_df], axis=1)

# reshape
table_explored_states = table_explored_states.reset_index()

table_explored_states.head()


# In[18]:


# calculate the stats of the number of explored states

table_explored_states_stats = table_explored_states.groupby(['model', 'property']).agg(['mean', 'min', 'max'])
table_explored_states_stats = table_explored_states_stats['explored_states']
table_explored_states_stats.head()


# # Examples

# In[19]:


create_figure_explored_states(table_explored_states, 'spool5')


# In[20]:


create_figure(df, "spool5")


# In[21]:


create_log_figure(table_time, table_error, "spool5", "pmc-sog_otfL_couv99-default_1_16", "pnml2lts-mc_dfs_1_16", True, open_logs_callback)


# # Generate Figures

# In[22]:


# models
models = df.model.unique()

# tools 
tools = df.tool.unique()


# In[23]:


# create all the figures of explored states

folder = os.path.join(OUTPUT_FOLDER, 'explored-states')
create_folder(folder)

for model in models:
    try:
        fig = create_figure_explored_states(table_explored_states, model)
        
        # save figures in html and pdf
        fig.write_html(os.path.join(folder, model + '.html'), include_plotlyjs='cdn')
        fig.write_image(os.path.join(folder, model + '.pdf'))
    except KeyError:
        print("Error: {} was not plotted".format(model))


# In[24]:


# create all the figures formula vs time

folder = os.path.join(OUTPUT_FOLDER, 'time-plots')
create_folder(folder)

for model in models:
    try:
        fig = create_figure(df, model)
        
        # save figures in html and pdf
        fig.write_html(os.path.join(folder, model + '.html'), include_plotlyjs='cdn')
        fig.write_image(os.path.join(folder, model + '.pdf'))
    except KeyError:
        print("Error: {} was not plotted".format(model))


# In[25]:


# create all the log figures

tools_pairs = [sorted(t) for t in (combinations(tools, 2))]

for plot, filter_method in plots.items():
    axes = list(filter(filter_method, tools_pairs))
    
    for model in models:
        folder = os.path.join(OUTPUT_FOLDER, plot, model)
        create_folder(folder)
        
        for axe in axes:
            try:
                show_strategy = plot == 'compare_couvreur_algorithm'
                fig = create_log_figure(table_time, table_error, model, axe[0], axe[1], show_strategy)
                
                # save figures in html and pdf
                figure_name = os.path.join(folder, '{}-{}-VS-{}-log'.format(model, axe[0], axe[1]))
                fig.write_html(figure_name + '.html', include_plotlyjs='cdn', post_script=OPEN_LOGS_CALLBACK_JS)
                fig.write_image(figure_name + '.pdf')
            except KeyError:
                print("Error: {} was not plotted".format(model))


# In[ ]:




